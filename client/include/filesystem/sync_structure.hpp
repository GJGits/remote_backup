#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unordered_map>

#include "../common/singleton.hpp"
#include "../pubsub/broker.hpp"
#include "file_entry.hpp"
#include "../common/json.hpp"

using json = nlohmann::json;

class SyncStructure : public Singleton<SyncStructure> {

private:
  friend class Singleton;
  std::shared_ptr<Broker> broker;
  std::unordered_map<std::string, std::shared_ptr<FileEntry>> structure;
  size_t last_check;

  SyncStructure() { broker = Broker::getInstance(); }

public:
  /**
   * Aggiorna il file client-struct.json
   **/
  void store() {
    std::ofstream o{"./config/client-struct.json"};
    json jstru = {{"entries", json::array()}, {"last_check", last_check}};
    for (const auto &[path, fentry] : structure) {
      json entry = {{"path", path},
                    {"last_change", fentry->get_last_change()},
                    {"status", fentry->get_status()},
                    {"producer", fentry->get_producer()},
                    {"nchunks", fentry->get_nchunks()}};
      jstru["entries"].push_back(entry);
    }
    o << jstru << "\n";
  }
  /**
   * Effettua restore struttura da file
   * */
  void restore() {
    std::ifstream i{"./config/client-struct.json"};
    json j;
    i >> j;
    last_check = j["last_check"].get<int>();
    for (size_t x = 0; x < j["entries"].size(); x++) {
      std::string path = j["entries"]["path"].get<std::string>();
      entry_producer producer =
          (entry_producer)j["entries"]["producer"].get<int>();
      size_t last_change = j["entries"]["last_change"].get<int>();
      entry_status status = (entry_status)j["entries"]["status"].get<int>();
      size_t nchunks = (size_t)j["entries"]["nchunks"].get<int>();
      std::shared_ptr<FileEntry> entry{
          new FileEntry{path, producer, nchunks, last_change, status}};
      add_entry(entry);
    }
  }
  /**
   * Effettua scan su filesystem ed aggiorna la struttura
   **/
  void update_from_fs() {
    for (const auto &p :
         std::filesystem::recursive_directory_iterator("./sync")) {
      if (p.is_regular_file()) {
        std::string path = p.path().string();
        entry_producer producer = entry_producer::folder;
        struct stat sb;
        stat(path.c_str(), &sb);
        size_t last_change = (size_t)sb.st_ctime;
        entry_status status = entry_status::new_;
        size_t nchunks =
            ceil((double)std::filesystem::file_size(path) / CHUNK_SIZE);
        std::shared_ptr<FileEntry> entry{
            new FileEntry{path, producer, nchunks, last_change, status}};
        if (last_change > last_check)
          add_entry(entry);
      }
    }
  }
  /**
   * Chiede aggiornamenti al server ed aggiorna la struttura.
   **/
  void update_from_remote() {
    // aggiornamenti fake da server (risultato get_list)
    json list{{"entries", json::array()}};
    for (size_t y = 0; y < list["entries"].size(); y++) {
      std::string path = list["entries"][y]["path"];
      size_t last_change = list["entries"][y]["last_remote_change"].get<int>();
      entry_status status =
          list["entries"][y]["command"].get<std::string>().compare("updated") ==
                  0
              ? entry_status::new_
              : entry_status::delete_;
      size_t nchunks = (size_t)list["entries"]["nchunks"].get<int>();
      std::shared_ptr<FileEntry> entry{new FileEntry{
          path, entry_producer::server, nchunks, last_change, status}};
      add_entry(entry);
    }
  }

  std::vector<std::string> get_paths() {
    std::vector<std::string> paths;
    for (const auto &[path, entry] : structure) {
      paths.push_back(path);
    }
    return paths;
  }

  size_t get_last_check() const {return last_check;}

  /**
   * Aggiunge entry alla struttura e permette di aggiornare
   * il conto dei chunk trasferiti
   * */
  void add_entry(const std::shared_ptr<FileEntry> &entry) {
    std::string path = entry->get_path();
    if (structure.find(path) == structure.end()) {
      structure[path] = entry;
    }
  }

  /**
   * Rimuove entry dalla struttura
   * */
  void remove_entry(const std::shared_ptr<FileEntry> &entry) {
    structure.erase(entry->get_path());
  }

  /**
   * Notifica cambiamenti sulla struttura
   **/
  void notify() {
    for (const auto &[path, entry] : structure) {
      if (entry->get_status() == entry_status::new_)
        broker->publish(Message{TOPIC::NEW_FILE, entry});
      if (entry->get_status() == entry_status::delete_)
        broker->publish(Message{TOPIC::FILE_DELETED, entry});
    }
  }
};
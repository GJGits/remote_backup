#pragma once
#include "../common/json.hpp"
#include "directory_entry.hpp"
#include "file_entry.hpp"
#include <string>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>



using json = nlohmann::json;

class SyncStructure {

private:
  inline static SyncStructure *instance = nullptr;

  void read_structure() {
    std::ifstream i("./config/client-struct.json");
    i >> structure;
  }

  void write_structure() {
    instance->hash_struct();
    std::ofstream o("./config/client-struct.json");
    o << structure << "\n";
  }

  void prune_structure() {
    for (json entry : structure["entries"]) {
      // 1. se il file e' gia' presente controlla se e'
      //    stato aggiornato offline.
      if (std::filesystem::exists(entry["path"])) {
        // todo: check timestamp ed hash per capire
        //       se il file e' stato aggiornato
          struct stat fileInfo;
          std::string name_file = entry["path"].get<std::string>();
          stat(name_file.c_str(), &fileInfo);
          std::ifstream input(name_file, std::ios::binary);
          std::vector<char> bytes((std::istreambuf_iterator<char>(input)),(std::istreambuf_iterator<char>()));
          if(fileInfo.st_mtim.tv_sec > entry["last_mod"] && entry["file_hash"] != Sha256::getSha256(bytes)){
              remove_entry(name_file);
              add_entry(name_file);
          }

      }

      // 2. se il file e' contenuto nella struttura, ma
      //    non appare nel filesystem allora il file
      //    e' stato eliminato
      if (!std::filesystem::exists(entry["path"])) {
        // todo: call watch_handler->inDelete(...);
      }
    }
  }

  void hash_struct() {
    std::clog << "size : " << structure["entries"].size() << "\n";
    if (structure["entries"].empty()) {
      structure["hashed_status"] = std::string{"empty_hashed_status"};
    } else {
      std::string entries_dump = structure["entries"].dump();
      std::vector<char> data(entries_dump.begin(), entries_dump.end());
      structure["hashed_status"] = structure["entries"].empty()
                                       ? std::string{"empty_hashed_status"}
                                       : Sha256::getSha256(data);
    }
  }

public:
  json structure;
  static SyncStructure *getInstance() {
    if (instance == nullptr) {
      instance = new SyncStructure{};
      // todo: se il file client-struct.json non esiste crealo
      instance->read_structure();
      // todo: testa instance->prune_structure();
      instance->prune_structure();
    }
    return instance;
  }

  void add_entry(const std::string &path) {
    auto iter =
        std::find_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) { return x["path"] == path; });
    if (iter == structure["entries"].end()) {
      FileEntry fentry{path};
      json entry = fentry.getEntry();
      structure["entries"].push_back(entry);
      instance->write_structure();
      // todo: handle creazione offline (handle_watcher->inCreate(...))
    }
  }

  void add_entry_directory(const std::string &path) {
    DirEntry dentry{path};
    json entry = dentry.getEntry();
    structure["entries"].push_back(entry);
    instance->write_structure();
  }

  void remove_entry(std::string &path) {
    if (!structure["entries"].empty()) {
      structure["entries"].erase(
          std::remove_if(structure["entries"].begin(),
                         structure["entries"].end(), [&](const json &x) {
                           return std::string{x["path"]}.compare(path) == 0;
                         }));
      instance->write_structure();
    }
  }
};

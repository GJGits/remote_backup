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

  void create_clientstructjson(){
      json j ;
      j["hashed_status"] = "empty_hashed_status";
      j["entries"] = json::array();
      std::ofstream o("./config/client-struct.json");
      o << std::setw(4) << j << std::endl;
  }

  void read_structure() {
    std::ifstream i("./config/client-struct.json");
    i >> structure;
  }

  void write_structure() {
    instance->hash_struct();
    std::ofstream o("./config/client-struct.json");
    o << structure << "\n";
    o.close();
  }

  void prune_structure() {
      auto it = structure["entries"].begin();
      while(it != structure["entries"].end()){
          json entry = *it;
      if (std::filesystem::exists(entry["path"])) {
          std::clog << "C'E' il file " << entry["path"] << "\n";
          struct stat fileInfo;
          std::string name_file = entry["path"].get<std::string>();
          stat(name_file.c_str(), &fileInfo);
          std::ifstream input(name_file, std::ios::binary);
          std::vector<char> bytes((std::istreambuf_iterator<char>(input)),(std::istreambuf_iterator<char>()));
          if(fileInfo.st_mtim.tv_sec > entry["last_mod"] && entry["file_hash"] != Sha256::getSha256(bytes)){
              std::clog << "Aggiorno il file " << entry["path"] << "\n";
              remove_entry(name_file);
              add_entry(name_file);
          }
          std::clog << "ci sono ancora\n " << entry["path"] << "\n";
          ++it;

      }

      // 2. se il file e' contenuto nella struttura, ma
      //    non appare nel filesystem allora il file
      //    e' stato eliminato
      else if (!std::filesystem::exists(entry["path"])) {
          std::string name_file = entry["path"].get<std::string>();
          remove_entry(name_file);
          std::clog << "cancello il file " << entry["path"] << "\n";

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
      if(!std::filesystem::exists("./config/client-struct.json"))
        instance->create_clientstructjson();
      instance->read_structure();
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
        std::clog << "PRIMA Sono in "<< path << "\n";

        structure["entries"].erase(
          std::remove_if(structure["entries"].begin(),
                         structure["entries"].end(), [&](const json &x) {
                           return std::string{x["path"]}.compare(path) == 0;
                         }));
        std::clog << "Dopo Sono in "<< path << "\n";

        instance->write_structure();
    }
  }
};

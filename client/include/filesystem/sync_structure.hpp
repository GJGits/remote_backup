#pragma once
#include "../common/json.hpp"
#include "file_entry.hpp"
#include "directory_entry.hpp"

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

  void hash_struct() {
    std::string entries_dump = structure["entries"].dump();
    std::vector<char> data(entries_dump.begin(), entries_dump.end());
    structure["hashed_status"] = structure["entries"].empty()
                                     ? std::string{"empty_hashed_status"}
                                     : Sha256::getSha256(data);
  }

public:
  json structure;
  static SyncStructure *getInstance() {
    if (instance == nullptr) {
      instance = new SyncStructure{};
      instance->read_structure();
    }
    return instance;
  }

  void add_entry(const std::string &path) {
    FileEntry fentry{path};
    json entry = fentry.getEntry();
    structure["entries"].push_back(entry);
    instance->write_structure();
  }
  
    void add_entry_directory(const std::string &path) {
    DirEntry dentry{path};
    json entry = dentry.getEntry();
    structure["entries"].push_back(entry);
    instance->write_structure();
  }

  void remove_entry(const std::string &path) {
    structure["entries"].erase(
        std::remove_if(structure["entries"].begin(), structure["entries"].end(),
                       [&](const json &x) {
                         return std::string{x["path"]}.compare(path) == 0;
                       }));
    instance->write_structure();
  }
};

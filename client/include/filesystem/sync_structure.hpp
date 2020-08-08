#pragma once
#include "../common/json.hpp"
#include "directory_entry.hpp"
#include "file_entry.hpp"

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
      instance->read_structure();
      //instance->prune_structure();
    }
    return instance;
  }

  void add_entry(const std::string &path) {
    auto iter =
        std::find_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) { return x["path"] == path; });
    if (iter != structure["entries"].end()) {
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
    structure["entries"].erase(
        std::remove_if(structure["entries"].begin(), structure["entries"].end(),
                       [&](const json &x) {
                         return std::string{x["path"]}.compare(path) == 0;
                       }));
    instance->write_structure();
  }
};

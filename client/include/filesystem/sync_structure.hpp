#pragma once
#include "../common/json.hpp"
#include "directory_entry.hpp"
#include "file_entry.hpp"
#include <string>

#include <boost/algorithm/string/replace.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

using json = nlohmann::json;

class SyncStructure {

private:
  inline static SyncStructure *instance = nullptr;

  void create_clientstructjson() {
    json j;
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

  void hash_struct() {
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
      if (!std::filesystem::exists("./config/client-struct.json") ||
          std::filesystem::is_empty("./config/client-struct.json"))
        instance->create_clientstructjson();
      instance->read_structure();
    }
    return instance;
  }

  json getEntries() const { return structure["entries"]; }

  // todo: trasformare questo in find_entry in modo tale da poter utilizzare
  //       il metodo in add_entry e remove_entry
  bool has_entry(const std::string &path) {
    auto iter =
        std::find_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) { return x["path"] == path; });
    return iter != structure["entries"].end();
  }

  /**
   * Aggiunge o aggiorna entry.
   *
   * @path: path del file aggiungere o aggiornare
   */

  void add_entry(const std::string &path) {
    auto iter =
        std::find_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) { return x["path"] == path; });
    FileEntry fentry{path};
    json new_entry = fentry.getEntry();
    if (new_entry["dim_last_chunk"] > 0) {
      if (iter != structure["entries"].end())
        remove_entry(path);
      structure["entries"].push_back(new_entry);
      instance->write_structure();
    }
  }

  /**
   * Variante di add_entry necessaria per un rename
   */
  void add_entry(const std::string &old_path, const std::string new_path) {
    auto iter =
        std::find_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) { return x["path"] == old_path; });
    if (iter != structure["entries"].end()) {
      remove_entry(old_path);
      add_entry(new_path);
    }
  }

  void remove_entry(const std::string &path) {
    if (!structure["entries"].empty()) {
      structure["entries"].erase(
          std::remove_if(structure["entries"].begin(),
                         structure["entries"].end(), [&](const json &x) {
                           return std::string{x["path"]}.compare(path) == 0;
                         }));
      instance->write_structure();
    }
  }

  /**
   * Elimina dal file di struct tutte le entry presenti che non esistono piu'
   * nel filesystem
   */
  void prune() {
    auto it = structure["entries"].begin();
    while (it != structure["entries"].end()) {
      json entry = *it;
      if (!std::filesystem::exists(entry["path"])) {
        remove_entry(entry["path"]);
      } else {
        it++;
      }
    }
  }

  // todo: dato il metodo prune valutare eliminazione
  void remove_sub_entries(std::string &path) {
    const std::string input = "^" + path + "/.*$";
    std::string output = boost::replace_all_copy(input, "/", "\\/");
    std::regex delete_reg{output};
    std::smatch match;

    auto it = structure["entries"].begin();
    while (it != structure["entries"].end()) {
      json entry = *it;
      std::string entry_path = entry["path"].get<std::string>();
      if (std::regex_match(entry_path, match, delete_reg)) {
        remove_entry(entry_path);
      } else {
        it++;
      }
    }
  }
};

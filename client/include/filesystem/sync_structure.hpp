#pragma once
#include "../common/duration.hpp"
#include "../common/json.hpp"
#include "directory_entry.hpp"
#include "file_entry.hpp"
#include <string>

#include <boost/algorithm/string/replace.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <vector>

using json = nlohmann::json;

class SyncStructure {

private:
  std::unique_ptr<json> structure;
  inline static SyncStructure *instance = nullptr;

  void create_clientstructjson() {
     json j;
    j["hashed_status"] = "empty_hashed_status";
    j["entries"] = json::array();
    std::ofstream o("./config/client-struct.json");
    o << std::setw(4) << j << std::endl;
  }

  void read_structure() {
    structure = std::make_unique<json>();
    std::ifstream i("./config/client-struct.json");
    i >> (*structure);
  }

  void hash_struct() {
    DurationLogger duration{"HASH_STRUCTURE"};
    if ((*structure)["entries"].empty()) {
      (*structure)["hashed_status"] = std::string{"empty_hashed_status"};
    } else {
      std::string entries_dump = (*structure)["entries"].dump();
      std::vector<char> data(entries_dump.begin(), entries_dump.end());
      (*structure)["hashed_status"] = (*structure)["entries"].empty()
                                          ? std::string{"empty_hashed_status"}
                                          : Sha256::getSha256(data);
    }
  }

public:
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

  void write_structure() {
    DurationLogger duration{"WRITE_STRUCTURE"};
    instance->hash_struct();
    std::ofstream o("./config/client-struct.json");
    o << (*structure) << "\n";
    o.close();
  }

  /**
   * Aggiunge o aggiorna entry.
   *
   * @path: path del file aggiungere o aggiornare
   */

  void add_entry(const std::string &path) {
    DurationLogger duration{"ADD_ENTRY"};
    auto iter =
        std::find_if((*structure)["entries"].begin(), (*structure)["entries"].end(),
                     [&](const json &x) { return x["path"] == path; });
    FileEntry fentry{path};
    json new_entry = fentry.getEntry();
    if (new_entry["dim_last_chunk"] > 0) {
      if (iter != (*structure)["entries"].end())
        remove_entry(path);
      (*structure)["entries"].push_back(new_entry);
    }
  }

  /**
   * Variante di add_entry necessaria per un rename
   */
  void rename_entry(const std::string path) {
    FileEntry fentry{path};
    json new_entry = fentry.getEntry();
    if (new_entry["dim_last_chunk"] > 0) {
      (*structure)["entries"].push_back(new_entry);
    }
  }

  void remove_entry(const std::string &path) {
    if (!(*structure)["entries"].empty()) {
      (*structure)["entries"].erase(
          std::remove_if((*structure)["entries"].begin(),
                         (*structure)["entries"].end(), [&](const json &x) {
                           return std::string{x["path"]}.compare(path) == 0;
                         }));
    }
  }

  /**
   * Elimina dal file di struct tutte le entry presenti che non esistono piu'
   * nel filesystem
   */
  void prune() {
    DurationLogger duration{"PRUNE"};
    auto it = (*structure)["entries"].begin();
    while (it != (*structure)["entries"].end()) {
      json entry = *it;
      if (!std::filesystem::exists(entry["path"])) {
        // todo: creare variante remove_entry che accetti iteratore, in questo modo
        //       non sara' piu' necessario fare ulteriore remove_if
        (*structure)["entries"].erase(it);
        //remove_entry(entry["path"]);
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

    auto it = (*structure)["entries"].begin();
    while (it != (*structure)["entries"].end()) {
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

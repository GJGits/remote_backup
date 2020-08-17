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
#include <mutex>
#include <regex>
#include <tuple>
#include <unordered_map>
#include <vector>

using json = nlohmann::json;

class SyncStructure {

private:
  int count;
  std::unordered_map<std::string, std::tuple<int, json>> entries;
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

    if (!(*structure)["entries"].empty()) {

      for (size_t i = 0; i < (*structure)["entries"].size(); i++) {

        json tmp = (*structure)["entries"][i];
        entries[tmp["path"]] = std::make_tuple((int)i, tmp);
        count++;
      }
    }
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

  FileEntry add_entry(const std::string &path) {
    DurationLogger duration{"ADD_ENTRY"};
    FileEntry fentry{path};
    json new_entry = fentry.getEntry();
    if (entries.find(path) == entries.end()) {
      if (new_entry["dim_last_chunk"] >= 0) {
        (*structure)["entries"].push_back(new_entry);
        entries[path] = std::make_tuple(count++, new_entry);
      }
    } else {
      int index = std::get<0>(entries[path]);
      if (new_entry["dim_last_chunk"] >= 0) {
        (*structure)["entries"][index] = new_entry;
      }
    }
    return fentry;
  }

  void replace_entry(const std::string &path) {
    int index = std::get<0>(entries[path]);
    FileEntry fentry{path};
    json new_entry = fentry.getEntry();
    if (new_entry["dim_last_chunk"] >= 0) {
      (*structure)["entries"][index] = new_entry;
    }
  }

  /**
   * Variante di add_entry necessaria per un rename
   */
  void rename_entry(const std::string &old_path, const std::string &new_path) {
    if (entries.find(old_path) != entries.end()) {
      int index = std::get<0>(entries[old_path]);
      json entry = std::get<1>(entries[old_path]);
      std::clog << "Last mod: " << entry["last_mod"] << "\n";
      (*structure)["entries"][index]["path"] = new_path;
      entries[new_path] = std::make_tuple(index, entry);
      entries.erase(old_path);
    }
  }

  void remove_entry(const std::string &path) {
    std::unique_lock lk{entries_mutex};
    int index = std::get<0>(entries[path]);
    entries.erase(path);
    (*structure)["entries"].erase(index);
    count--;
  }
  /**
   * Elimina dal file di struct tutte le entry presenti che non esistono piu'
   * nel filesystem
   */
  std::vector<std::string> prune() {
    std::vector<std::string> result;
    std::unique_lock lk{entries_mutex};
    DurationLogger duration{"PRUNE"};
    auto it = (*structure)["entries"].begin();
    while (it != (*structure)["entries"].end()) {
      json entry = *it;
      if (!std::filesystem::exists(entry["path"])) {
        result.push_back(entry["path"]);
        entries.erase(entry["path"]);
        (*structure)["entries"].erase(it);
        count--;
      } else {
        it++;
      }
    }
    return result;
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

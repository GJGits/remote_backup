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
  bool dirty;
  bool finish;
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
    if (structure.get() == nullptr || (*structure).empty()) {
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
    }
    instance->read_structure();
    return instance;
  }

  void write_structure() {
    if (dirty) {
      DurationLogger duration{"WRITE_STRUCTURE"};
      instance->hash_struct();
      std::ofstream o("./config/client-struct.json");
      o << (*structure) << "\n";
      o.close();
      (*structure).clear();
      dirty = false;
    }
  }

  bool has_entry(const std::string &path) {

    if (entries.find(path) == entries.end()) {
      return false;
    }
    return std::get<1>(entries[path])["validity"];
  }

  bool is_updated(const std::string &path, int last_mod) {
    if (has_entry(path)) {
      return std::get<1>(entries[path])["last_mod"] < last_mod;
    }
    return true;
  }

  /**
   * Aggiunge o aggiorna entry.
   *
   * @path: path del file aggiungere o aggiornare
   */

  void add_entry(const json &entry) {
    DurationLogger duration{"ADD_ENTRY"};
    std::clog << "e0\n";
    if (entry["size"] >= 0) {
      std::clog << "e1\n";
      if (entries.find(entry["path"]) == entries.end()) {
        std::clog << "e2\n";
        dirty = true;
        (*structure)["entries"].push_back(entry);
        entries[entry["path"]] = std::make_tuple(count++, entry);
      } else {
        std::clog << "e2.1\n";
        json chunk = entry["chunks"][0]; // entry viene inviata sempre con un solo chunk
        int index = std::get<0>(entries[entry["path"]]);
        bool found = false;
        for (size_t i = 0; i < (*structure)["entries"][index]["chunks"].size();
             i++) {
          if ((*structure)["entries"][index]["chunks"][i]["id"] == chunk["id"]) {
            found = true;
            (*structure)["entries"][index]["chunks"][i] = chunk;
          }
        }
        if (!found) {
          (*structure)["entries"][index]["chunks"].push_back(chunk);
        }
      }
    }
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
      dirty = true;
    }
  }

  void remove_entry(const std::string &path) {
    int index = std::get<0>(entries[path]);
    entries.erase(path);
    (*structure)["entries"].erase(index);
    count--;
    dirty = true;
  }

  std::vector<std::string> prune() {
    std::vector<std::string> result;
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
    if (!result.empty())
      dirty = true;
    return result;
  }

  void increase_counter() { count++; }
  void decrease_counter() { count--; }
  void reset_counter() { count = 0; }
  int get_count() const { return count; }
};

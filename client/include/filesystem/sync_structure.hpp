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

  bool has_entry(const std::string &path) {
    auto iter =
        std::find_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) { return x["path"] == path; });
    return iter != structure["entries"].end();
  }

  void add_entry(const std::string &path) {
    auto iter =
        std::find_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) { return x["path"] == path; });
    if (iter == structure["entries"].end()) {
      FileEntry fentry{path};
      json entry = fentry.getEntry();
      if (entry["dim_last_chunk"] > 0) {
        structure["entries"].push_back(entry);
        instance->write_structure();
      }
    }
  }

  void update_entry(const std::string &path) {
    auto iter =
        std::find_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) { return x["path"] == path; });
    if (iter != structure["entries"].end()) {
      FileEntry fentry{path};
      json entry = fentry.getEntry();
      json old_entry = *iter;
      if (old_entry["file_hash"] != entry["file_hash"]) {
        remove_entry(path);
        add_entry(path);
      }
    } else {
      add_entry(path);
    }
  }

  void update_entry(const std::string &old_path, const std::string &new_path) {
    auto iter =
        std::find_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) { return x["path"] == old_path; });
    if (iter != structure["entries"].end()) {
      FileEntry fentry{new_path};
      json entry = fentry.getEntry();
      json old_entry = *iter;
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

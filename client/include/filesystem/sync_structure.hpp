#pragma once

#include <boost/algorithm/string/replace.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../common/sha256.hpp"
#include "../common/duration.hpp"
#include "../common/json.hpp"

using json = nlohmann::json;

class SyncStructure {

private:
  std::unordered_map<std::string, json> entries;
  inline static std::shared_ptr<SyncStructure> instance{nullptr};
  
  void create_structure();
  void read_structure();

public:
  static std::shared_ptr<SyncStructure> getInstance();
  void write_structure();
  void add_chunk(const json &chunk);
  void replace_chunk(const json &chunk);
  void delete_chunk(const json &chunk);
  bool has_entry(const std::string &path);
  void delete_entry(const json &entry);
  void rename_entry(const json &entry);
  std::vector<std::string> get_entry_hashes(const std::string &path);
  std::vector<std::string> get_paths();
  size_t get_last_mod(const std::string &path);
};
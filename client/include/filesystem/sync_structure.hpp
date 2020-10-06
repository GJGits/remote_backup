#pragma once

#include <boost/algorithm/string/replace.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>

#include "../common/duration.hpp"
#include "../common/json.hpp"
#include "../common/sha256.hpp"

using json = nlohmann::json;

class SyncStructure {

private:
  std::unordered_map<std::string, json> entries;
  inline static std::shared_ptr<SyncStructure> instance{nullptr};
  std::mutex m;
  bool dirty;
  int last_check = 0;

  void create_structure();
  void read_structure();

public:
  static std::shared_ptr<SyncStructure> getInstance();
  void write_structure();
  void add_chunk(const json &chunk);
  bool has_entry(const std::string &path);
  void reset_chunks(const std::string &path);
  void delete_entry(const json &entry);
  void rename_entry(const json &entry);
  std::vector<std::string> get_entry_hashes(const std::string &path);
  std::vector<std::string> get_paths();
  size_t get_last_mod(const std::string &path);
  int get_last_check();
};
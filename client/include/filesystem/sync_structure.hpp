#pragma once

#include <boost/algorithm/string/replace.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../common/duration.hpp"
#include "../common/json.hpp"

using json = nlohmann::json;

class SyncStructure {

private:
  bool dirty;
  int count;
  std::unordered_map<std::string, std::tuple<int, json>> entries;
  std::unique_ptr<json> structure;
  inline static std::shared_ptr<SyncStructure> instance{nullptr};

  void create_structure();
  void read_structure();
  void write_structure();

public:
  static std::shared_ptr<SyncStructure> getInstance();
  void add_chunk(const json &chunk);
  void replace_chunk(const json &chunk);
  void delete_chunk(const json &chunk);
  bool has_entry(const std::string &path);
  void delete_entry(const json &entry);
  std::vector<std::string> get_paths() const;
  size_t get_last_mod(const std::string &path) const;
};
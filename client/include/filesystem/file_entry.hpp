#pragma once

#include <string>
#include <filesystem>
#include <math.h>
#include <fstream>

#include "../common/json.hpp"

using json = nlohmann::json;

class FileEntry {
private:
  json entry;
  std::string path;
  uintmax_t size;
  size_t nchunks;
  std::ifstream in;
  size_t read_count;

public:
  FileEntry(const std::string &path);
  bool has_chunk();
  std::shared_ptr<char[]> next_chunk();
  void add_chunk(json &chunk);
  void clear_chunks();
  json get_json_representation();
};
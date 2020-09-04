#pragma once

#include <string>
#include <filesystem>
#include <math.h>

#include "../common/json.hpp"
#include "../filesystem/chunk.hpp"

using json = nlohmann::json;

class FileEntry {
private:
  json entry;
  std::string path;
  uintmax_t size;
  size_t nchunks;

public:
  FileEntry(const std::string &path);
  uintmax_t get_size();
  void add_chunk(json &chunk);
  size_t get_nchunks();
  void clear_chunks();
  json get_json_representation();
};
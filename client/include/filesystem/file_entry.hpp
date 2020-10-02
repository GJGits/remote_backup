#pragma once

#include <string>
#include <filesystem>
#include <math.h>
#include <fstream>
#include <tuple>

#include "../common/sha256.hpp"
#include "../common/json.hpp"

using json = nlohmann::json;

#define CHUNK_SIZE 4096

class FileEntry {
private:
  json entry;
  std::string path;
  uintmax_t size;
  size_t nchunks;
  std::ifstream in;
  size_t read_count;
  std::shared_ptr<char[]> buffer;

public:
  FileEntry(const std::string &path);
  bool has_chunk();
  std::tuple<std::shared_ptr<char[]>, size_t> next_chunk();
  void clear_chunks();
  json get_json_representation();
  size_t get_nchunks();
};
#pragma once

#include "../common/sha256.hpp"
#include "../common/json.hpp"
#include <fstream>
#include <memory>
#include <string>

using json = nlohmann::json;

#define CHUNK_SIZE 2097152 // chunk size espressa in byte

class Chunk {

private:
  std::string path;
  json j_chunk;
  std::unique_ptr<char[]> buffer;

public:
  /* Read constructor */
  Chunk(const std::string &path, int id);
  /* Write constructor */
  Chunk(const std::string &path, int id, std::unique_ptr<char[]> &&buffer);
  void read(size_t dim);
  void write(size_t dim);
  json get_json_representation() const;
};
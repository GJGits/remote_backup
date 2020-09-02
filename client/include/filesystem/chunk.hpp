#pragma once

#include <fstream>
#include <memory>
#include <string>

#include "../common/json.hpp"
#include "../common/sha256.hpp"

using json = nlohmann::json;

#define CHUNK_SIZE 2097152 // chunk size espressa in byte

class Chunk {

private:
  std::ifstream istream;
  std::ofstream ostream;
  json j_chunk;
  std::unique_ptr<char[]> buffer;

public:
  /* Read constructor */
  Chunk(std::ifstream &&istream);
  /* Write constructor */
  Chunk(std::ofstream &&ostream, std::unique_ptr<char[]> &&buffer);
  void read(size_t id, size_t dim);
  void write(size_t id, size_t dim);
  json get_json_representation() const;
  ~Chunk() {
    if (istream.is_open())
      istream.close();
    if (ostream.is_open())
      ostream.close();
  }
};
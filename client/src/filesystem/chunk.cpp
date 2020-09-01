#include "../../include/filesystem/chunk.hpp"

Chunk::Chunk(const std::string &path, int id) : path{path} {
  j_chunk["id"] = id;
}

Chunk::Chunk(const std::string &path, int id, std::unique_ptr<char[]> &&buffer)
    : path{path}, buffer{std::move(buffer)} {
  j_chunk["id"] = id;
}

void Chunk::read(size_t dim) {
  std::ifstream input_stream(path, std::ios::binary);
  input_stream.seekg(j_chunk["id"] * CHUNK_SIZE);
  buffer = std::move(std::unique_ptr<char[]>(new char[CHUNK_SIZE + 1]));
  memset(buffer.get(), '\0', CHUNK_SIZE);
  input_stream.read(buffer.get(), dim); // todo: check on reads
  std::vector<char> chunk_buf{buffer.get(), buffer.get() + dim};
  std::string hash_chunk = Sha256::getSha256(chunk_buf);
  j_chunk["hash"] = hash_chunk;
}

void Chunk::write(size_t dim) {}

json Chunk::get_json_representation() const {return j_chunk;};
#include "../../include/filesystem/chunk.hpp"

Chunk::Chunk(std::ifstream &&istream)
    : istream{std::move(istream)}, buffer{std::move(std::unique_ptr<char[]>(
                                       new char[CHUNK_SIZE + 1]))} {
}

Chunk::Chunk(std::ofstream &&ostream, std::unique_ptr<char[]> &&buffer)
    : ostream{std::move(ostream)}, buffer{std::move(buffer)} {
}

void Chunk::read(size_t id, size_t dim) {
  j_chunk["id"] = id;
  istream.seekg(id * CHUNK_SIZE);
  memset(buffer.get(), '\0', CHUNK_SIZE);
  istream.read(buffer.get(), dim); // todo: check on reads
  std::vector<char> chunk_buf{buffer.get(), buffer.get() + dim};
  std::string hash_chunk = Sha256::getSha256(chunk_buf);
  j_chunk["hash"] = hash_chunk;
}

void Chunk::write(size_t id, size_t dim) {}

json Chunk::get_json_representation() { return j_chunk; };
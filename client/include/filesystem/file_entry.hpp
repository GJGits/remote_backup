#pragma once
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

#include "../common/sha256.hpp"

#define CHUNK_SIZE 100 // chunk size espressa in byte

using namespace std::chrono_literals;

class FileEntry {
private:
  std::string path;
  std::time_t last_mod;
  std::string file_hash;
  std::vector<std::string> chunks;
  int dim_last_chunk;

public:
  FileEntry(const std::string &path) : path{path} {
    std::ifstream input(path, std::ios::binary);
    std::vector<char> bytes((std::istreambuf_iterator<char>(input)),
                            (std::istreambuf_iterator<char>()));
    file_hash = Sha256::getSha256(bytes);
    int fsize = (int)std::filesystem::file_size(path);
    if (fsize <= CHUNK_SIZE) {
      chunks.push_back(file_hash);
      dim_last_chunk = fsize < CHUNK_SIZE ? fsize : 0;
    } else {
      int num_of_chunks = ceil(fsize / CHUNK_SIZE);
      for (int i = 0; i < num_of_chunks - 1; i++) {
        std::vector<char> chunk(
            (std::istreambuf_iterator<char>(input)),
            (std::istreambuf_iterator<char>(input.seekg(10))));
        std::string chunk_hash = Sha256::getSha256(chunk);
        chunks.push_back(chunk_hash);
      }
    }
  }

  std::string getFileHash() { return file_hash; }
  std::vector<std::string> getChunks() { return chunks; }
  int getLastChunk() { return dim_last_chunk; }
};
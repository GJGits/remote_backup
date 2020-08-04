#pragma once
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
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
      // todo: aggiungere logica chunk per file con piu' chunks
    }
    struct stat fileInfo;
    stat(path.c_str(), &fileInfo);
    last_mod = fileInfo.st_mtim.tv_sec;
  }

  std::string getFileHash() { return file_hash; }
  std::vector<std::string> getChunks() { return chunks; }
  int getLastChunk() { return dim_last_chunk; }
  time_t getLastMod() { return last_mod; }
};
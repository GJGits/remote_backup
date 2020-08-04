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

#include "../common/json.hpp"

#include "../common/sha256.hpp"

#define CHUNK_SIZE 100 // chunk size espressa in byte

using namespace std::chrono_literals;

class FileEntry {
private:
  std::string path;
  json entry;

  void fill_file_hash() {
    std::ifstream input(path, std::ios::binary);
    std::vector<char> bytes((std::istreambuf_iterator<char>(input)),
                            (std::istreambuf_iterator<char>()));
    entry["file_hash"] = Sha256::getSha256(bytes);
  }

  void fill_chunks() {
    int fsize = (int)std::filesystem::file_size(path);
    if (fsize <= CHUNK_SIZE) {
      entry["chunks"].push_back(entry["file_hash"]);
      entry["dim_last_chunk"] = fsize < CHUNK_SIZE ? fsize : 0;
    } else {
      // todo: aggiungere logica chunk per file con piu' chunks
    }
  }

  void fill_last_mod() {
    struct stat fileInfo;
    stat(path.c_str(), &fileInfo);
    entry["last_mod"] = fileInfo.st_mtim.tv_sec;
  }

public:
  FileEntry(const std::string &path) : path{path} {
    entry["path"] = path;
    fill_file_hash();
    fill_chunks();
    fill_last_mod();
  }

  json getEntry() {return entry;}

};
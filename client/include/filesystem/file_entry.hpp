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

#define CHUNK_SIZE 524288 // chunk size espressa in byte

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
      int seek_pos = 0;
      std::ifstream file(path, std::ios::binary);
      char read_buf[CHUNK_SIZE];
      while (seek_pos < fsize) {
        memset(read_buf, '\0', CHUNK_SIZE);
        file.seekg(seek_pos);
        size_t to_read =
            (fsize - seek_pos) >= CHUNK_SIZE ? CHUNK_SIZE : (fsize - seek_pos);
        file.read(read_buf, to_read); // todo: check su read
        std::vector<char> chunk_buf{read_buf, read_buf + to_read};
        entry["chunks"].push_back(Sha256::getSha256(chunk_buf));
        seek_pos += to_read;
        if (to_read < CHUNK_SIZE)
          entry["dim_last_chunk"] = to_read;
      }
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

  json getEntry() { return entry; }
};

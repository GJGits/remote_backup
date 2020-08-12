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
#include <memory>

#include "../common/duration.hpp"
#include "../common/json.hpp"

#include "../common/sha256.hpp"

#define CHUNK_SIZE 2097152 // chunk size espressa in byte

using namespace std::chrono_literals;

class FileEntry {
private:
  std::string path;
  json entry;
  std::string hash_concats;

  void fill_file_hash() {
    DurationLogger duration{"FILL_FILE_HASH"};
    std::ifstream input(path, std::ios::binary);
    std::vector<char> bytes;
    std::copy(hash_concats.begin(), hash_concats.end(),
              std::back_inserter(bytes));
    entry["file_hash"] = Sha256::getSha256(bytes);
  }

  void fill_chunks() {
    DurationLogger duration{"FILL_CHUNKS"};
    int fsize = (int)std::filesystem::file_size(path);
    if (fsize <= CHUNK_SIZE) {
      entry["chunks"].push_back(entry["file_hash"]);
      entry["dim_last_chunk"] = fsize < CHUNK_SIZE ? fsize : 0;
    } else {
      int seek_pos = 0;
      std::ifstream file(path, std::ios::binary);
      std::unique_ptr<char[]> read_buf(new char[CHUNK_SIZE + 1]);
      //char *read_buf = new char[CHUNK_SIZE + 1];
      while (seek_pos < fsize) {
        memset(read_buf.get(), '\0', CHUNK_SIZE);
        file.seekg(seek_pos);
        size_t to_read =
            (fsize - seek_pos) >= CHUNK_SIZE ? CHUNK_SIZE : (fsize - seek_pos);
        file.read(read_buf.get(), to_read); // todo: check su read
        std::vector<char> chunk_buf{read_buf.get(), read_buf.get() + to_read};
        std::string hash_chunk = std::move(Sha256::getSha256(chunk_buf));
        entry["chunks"].push_back(hash_chunk);
        hash_concats += hash_chunk;
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
    fill_chunks();
    fill_file_hash();
    //fill_last_mod();
  }

  json getEntry() { return entry; }
};

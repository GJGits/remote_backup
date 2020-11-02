#pragma once

#include <filesystem>
#include <fstream>
#include <math.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>

#include "../common/base64.hpp"
#include "../common/duration.hpp"
#include "../common/sha256.hpp"

#define CHUNK_SIZE 65536 // 64KB

enum entry_status { synced, new_, delete_ };
enum entry_producer { local, server };

class FileEntry {
private:
  std::string path;
  entry_producer producer;
  uintmax_t size;
  size_t nchunks;
  size_t last_change;
  entry_status status;
  size_t read_count;
  std::shared_ptr<char[]> buffer;
  std::tuple<size_t, std::string> last_move;

public:
  /**
   * Costruttore vuoto per dichiarazioni e compatibilita con alcune strutture
   **/
  FileEntry();

  /**
   * Costruttore per nuova entry
   **/
  FileEntry(const std::string &path, entry_producer producer,
            entry_status status);

  /**
   * Costruttore per entry gia esistente
   **/
  FileEntry(const std::string &path, entry_producer producer, size_t nchunks,
            size_t last_change, entry_status status);

  bool has_chunk();
  void set_status(entry_status status) { this->status = status; }
  std::tuple<std::shared_ptr<char[]>, size_t> next_chunk();
  size_t get_nchunks() const;
  size_t get_last_change() const { return last_change; }
  entry_status get_status() const { return status; }
  std::string get_path() const { return path; }
  entry_producer get_producer() const { return producer; }
  std::string to_string() {
    return std::to_string(std::get<0>(last_move)) + "/" +
           std::get<1>(last_move) + "/" + std::to_string(nchunks) + "/" +
           macaron::Base64::Encode(path) + "/" + std::to_string(last_change);
  }
  void update_read_count() { read_count++; }
};
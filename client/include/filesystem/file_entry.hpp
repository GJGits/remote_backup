#pragma once

#include <filesystem>
#include <fstream>
#include <math.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>
#include <optional>

#include "../common/base64.hpp"
#include "../common/duration.hpp"
#include "../common/json.hpp"
#include "../common/sha256.hpp"
#include "../http/rest_client.hpp"

#define CHUNK_SIZE 65536 // 64KB

using json = nlohmann::json;

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
  void set_producer(entry_producer producer);
  void set_status(entry_status status);
  std::tuple<std::shared_ptr<char[]>, size_t> next_chunk();
  void set_read_count(size_t read_count) const;
  void set_nchunks(size_t nchunks) const;
  size_t get_nchunks() const;
  size_t get_last_change() const;
  entry_status get_status() const;
  std::string get_path() const;
  entry_producer get_producer() const;
  std::tuple<size_t, std::string> get_last_move() const;
  std::string to_string() {
    return std::to_string(std::get<0>(last_move)) + "/" +
           std::get<1>(last_move) + "/" + std::to_string(nchunks) + "/" +
           macaron::Base64::Encode(path) + "/" + std::to_string(last_change);
  }
  void retrieve_chunk();
  void update_read_count() { read_count++; }
  json to_json();
};
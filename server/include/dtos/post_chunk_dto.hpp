#pragma once

#include <iostream>
#include "../http/request.hpp"
#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "json-serializable.hpp"

class PostChunkDTO {
private:
  std::string username;
  int chunk_id;
  int chunk_size;
  std::string file_dir;
  std::string file_path;
  std::string chunk_hash;
  int timestamp_locale;
  std::shared_ptr<std::vector<char>> chunk_body{new std::vector<char>{}};

public:
  PostChunkDTO(){};
  std::string getusername() const;
  int getchunk_id() const;
  int getchunk_size() const;
  std::string getchunk_hash() const;
  std::string getfile_path() const;
  std::string getfile_dir() const;
  int gettimestamp_locale() const;
  std::shared_ptr<std::vector<char>> getchunk_body() const;

  const void fill(const http::server::request &req);
};
#pragma once

#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "../http/request.hpp"
#include "json-serializable.hpp"
#include "subject.hpp"
#include <iostream>

class PostChunkDTO {
private:
  Subject subject;
  int chunk_id;
  int chunk_size;
  int num_chunks;
  std::string file_name;
  std::string file_path;
  std::string chunk_hash;
  int timestamp_locale;
  std::shared_ptr<std::vector<char>> chunk_body{new std::vector<char>{}};

public:
  PostChunkDTO(const Subject &subject) : subject{subject} {}
  Subject get_subject() const;
  int getchunk_id() const;
  int getchunk_size() const;
  std::string getchunk_hash() const;
  int getnum_chunks() const;
  std::string getfile_path() const;
  std::string getfile_name() const;
  int gettimestamp_locale() const;
  std::shared_ptr<std::vector<char>> getchunk_body() const;

  const void fill(const http::server::request &req);
};
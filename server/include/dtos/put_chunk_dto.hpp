#pragma once
#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "../http/request.hpp"
#include "json-serializable.hpp"
#include "subject.hpp"

class PutChunkDTO : public Subject {
private:
  std::string username;
  int chunk_id;
  int chunk_size;
  std::string chunk_hash;
  std::string file_dir;
  std::string file_path;
  std::shared_ptr<std::vector<char>> chunk_body;
  int timestamp_locale;

public:
  PutChunkDTO(const Subject &subject) : Subject{subject} {};
  std::string getusername() const;
  int getchunk_id() const;
  int getchunk_size() const;
  std::string getchunk_hash() const;
  std::string getfile_dir() const;
  std::string getfile_path() const;
  std::shared_ptr<std::vector<char>> getchunk_body() const;
  int gettimestamp_locale() const;
  const void fill(const http::server::request &req);
};
#pragma once

#include <iostream>

#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "json-serializable.hpp"

class PostChunkDTO {
private:
  std::string username;
  int chunk_id;
  int chunk_size;
  std::string file_path;
  std::string full_file_path;
  std::string chunk_hash;
  std::string timestamp_locale;
  std::vector<char> chunk_body;

public:
  PostChunkDTO(){};
  std::string getusername() const;
  int getchunk_id() const;
  int getchunk_size() const;
  std::string getchunk_hash() const;
  std::string getfile_path() const;
  std::string get_full_file_path() const;
  std::string gettimestamp_locale() const;
  std::vector<char> getchunk_body() const;

  const void fill(std::string requri,std::vector<char> reqbody);
};
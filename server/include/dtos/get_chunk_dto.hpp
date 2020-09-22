#pragma once
#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "json-serializable.hpp"
#include "subject.hpp"

class GetChunkDTO {
private:
  Subject subject;
  std::string file_name;
  int chunk_id;
  int chunk_size;
  std::string file_path;

public:
  GetChunkDTO(const Subject &subject): subject{subject} {}
  GetChunkDTO(){}
  Subject get_subject() const;
  int getchunk_id() const;
  int getchunk_size() const;
  std::string getfile_path() const;
  std::string getfile_name() const;
  const void fill(const std::string &requri);
};
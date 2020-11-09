#pragma once
#include "../common/utility.hpp"
#include "subject.hpp"
#include "../common/constants.hpp"
#include <memory>
class GetChunkDTO {
private:
  Subject subject;
  std::string file_name;
  int chunk_id;
  std::string file_path;
  std::shared_ptr<std::array<char, CHUNK_SIZE>> rep_content{nullptr};

public:
  GetChunkDTO(const Subject &subject) : subject{subject} {}
  GetChunkDTO() {}
  Subject get_subject() const;
  int getchunk_id() const;
  std::string getfile_path() const;
  std::string getfile_name() const;
  void link_content_buffer(std::shared_ptr<std::array<char, CHUNK_SIZE>> &buff);
  std::shared_ptr<std::array<char, CHUNK_SIZE>> get_content_buffer() const;
  const void fill(const std::string &requri);
};
#include "../../include/dtos/put_chunk_dto.hpp"

Subject PutChunkDTO::get_subject() const { return subject; }
int PutChunkDTO::getchunk_id() const { return chunk_id; }
int PutChunkDTO::getchunk_size() const { return chunk_size; }
std::string PutChunkDTO::getchunk_hash() const { return chunk_hash; }
std::string PutChunkDTO::getfile_name() const { return file_name; }
std::string PutChunkDTO::getfile_path() const { return file_path; }
std::shared_ptr<std::vector<char>> PutChunkDTO::getchunk_body() const {
  return chunk_body;
}
int PutChunkDTO::gettimestamp_locale() const { return timestamp_locale; }

const void PutChunkDTO::fill(const http::server::request &req) {
  std::vector<std::string> uri_elements = Utility::split(req.uri, '/');
  chunk_id = std::stoi(uri_elements[2]);
  chunk_size = std::stoi(uri_elements[3]);
  chunk_hash = std::move(uri_elements[4]);
  file_name = std::move(uri_elements[5]);
  file_path = "../../filesystem/" + get_subject().get_sub() + "/" + file_name;
  timestamp_locale = std::stoi(uri_elements[6]);
  std::move(req.body.get()->begin(), req.body.get()->end(),
            std::back_inserter(*chunk_body));
}
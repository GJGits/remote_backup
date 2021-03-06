#include "../../include/dtos/post_chunk_dto.hpp"

Subject PostChunkDTO::get_subject() const {return subject;}
int PostChunkDTO::getchunk_id() const { return chunk_id; }
int PostChunkDTO::getchunk_size() const { return chunk_size; }
std::string PostChunkDTO::getchunk_hash() const { return chunk_hash; }
int PostChunkDTO::getnum_chunks() const { return num_chunks; }
std::string PostChunkDTO::getfile_path() const { return file_path; }
std::string PostChunkDTO::getfile_name() const { return file_name; }
std::shared_ptr<std::vector<char>> PostChunkDTO::getchunk_body() const {
  return chunk_body;
}

const void PostChunkDTO::fill(const http::server::request &req, size_t ck_size) {
  std::vector<std::string> uri_elements = Utility::split(req.uri, '/');
  chunk_id = std::stoi(uri_elements[2]);
  chunk_size = (int)ck_size;
  chunk_hash = std::move(uri_elements[3]);
  num_chunks = std::stoi(uri_elements[4]);
  file_name = std::move(uri_elements[5]);
  file_path = "../../filesystem/" + get_subject().get_sub() + "/" + file_name;
  std::move(req.body.get()->begin(), req.body.get()->end(),
            std::back_inserter(*chunk_body));
}



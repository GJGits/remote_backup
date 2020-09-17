#include "../../include/dtos/put_chunk_dto.hpp"

std::string PutChunkDTO::getusername() const { return username; }
int PutChunkDTO::getchunk_id() const { return chunk_id; }
int PutChunkDTO::getchunk_size() const { return chunk_size; }
std::string PutChunkDTO::getchunk_hash() const { return chunk_hash; }
std::string PutChunkDTO::getfile_dir() const { return file_dir; }
std::string PutChunkDTO::getfile_path() const { return file_path; }
std::shared_ptr<std::vector<char>> PutChunkDTO::getchunk_body() const {
  return chunk_body;
}
int PutChunkDTO::gettimestamp_locale() const { return timestamp_locale; }

const void
PutChunkDTO::fill(const http::server::request
                      &req) { // todo: passarlo per riferimento, non per copia
  std::vector<std::string> uri_elements = Utility::split(req.uri, '/');
  username = std::move(uri_elements[2]);
  chunk_id = std::stoi(uri_elements[3]);
  chunk_size = std::stoi(uri_elements[4]);
  chunk_hash = std::move(uri_elements[5]);
  file_path = "../../filesystem/" + username + "/" + uri_elements[6];
  std::vector<std::string> travers_dir = Utility::split(file_path, '/');
  for (size_t i = 0; i < (travers_dir.size() - 2); i++)
    file_dir += travers_dir[i] + "/";
  file_dir += travers_dir[travers_dir.size() - 2];
  timestamp_locale = std::stoi(uri_elements[7]);
  std::move(req.body.get()->begin(), req.body.get()->end(),
            std::back_inserter(*chunk_body));
}
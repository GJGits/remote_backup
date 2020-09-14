#include "../../include/dtos/post_chunk_dto.hpp"

std::string PostChunkDTO::getusername() const { return username; }
int PostChunkDTO::getchunk_id() const { return chunk_id; }
int PostChunkDTO::getchunk_size() const { return chunk_size; }
std::string PostChunkDTO::getchunk_hash() const { return chunk_hash; }
std::string PostChunkDTO::getfile_path() const { return file_path; }
std::string PostChunkDTO::get_full_file_path() const { return full_file_path; }
std::string PostChunkDTO::gettimestamp_locale() const { return timestamp_locale; }
std::shared_ptr<std::vector<char>> PostChunkDTO::getchunk_body() const { return chunk_body; }

const void PostChunkDTO::fill(const http::server::request &req) {
    std::vector<std::string> uri_elements = Utility::split(req.uri, '/');
    username = uri_elements[2];
    chunk_id = std::stoi(uri_elements[3]);
    chunk_size = std::stoi(uri_elements[4]);
    chunk_hash = uri_elements[5];
    file_path = macaron::Base64::Decode(uri_elements[6]);
    full_file_path = "../../filesystem/"+username+"/"+file_path;
    timestamp_locale = uri_elements[7];

    std::move(req.body.get()->begin(), req.body.get()->end(),std::back_inserter(*chunk_body));

}
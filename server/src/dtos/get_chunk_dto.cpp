#include "../../include/dtos/get_chunk_dto.hpp"

std::string GetChunkDTO::getusername() const { return username; }
int GetChunkDTO::getchunk_id() const { return chunk_id; }
int GetChunkDTO::getchunk_size() const { return chunk_size; }
std::string GetChunkDTO::getfile_path() const { return file_path; }


const void GetChunkDTO::fill(const std::string &requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    username = std::move(uri_elements[2]);
    chunk_id = std::stoi(uri_elements[3]);
    chunk_size = std::stoi(uri_elements[4]);
    file_path = macaron::Base64::Decode(std::move(uri_elements[5]));
}

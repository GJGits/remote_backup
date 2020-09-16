#include "../../include/dtos/delete_chunk_dto.hpp"

std::string DeleteChunkDTO::getusername() const { return username; }
int DeleteChunkDTO::getchunk_id() const { return chunk_id; }
std::string DeleteChunkDTO::getfile_path() const { return file_path; }
int DeleteChunkDTO::gettimestamp_locale() const { return timestamp_locale; }

const void DeleteChunkDTO::fill( std::string requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    username = std::move(uri_elements[2]);
    chunk_id = std::stoi(uri_elements[3]);
    file_path = macaron::Base64::Decode(std::move(uri_elements[4]));
    timestamp_locale = std::stoi(uri_elements[5]);

}
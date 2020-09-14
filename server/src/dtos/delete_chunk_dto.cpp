#include "../../include/dtos/delete_chunk_dto.hpp"

std::string DeleteChunkDTO::getusername() const { return username; }
int DeleteChunkDTO::getchunk_id() const { return chunk_id; }
std::string DeleteChunkDTO::getfile_path() const { return file_path; }
std::string DeleteChunkDTO::get_full_file_path() const { return full_file_path; }
std::string DeleteChunkDTO::gettimestamp_locale() const { return timestamp_locale; }

const void DeleteChunkDTO::fill( std::string requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    username = uri_elements[2];
    chunk_id = std::stoi(uri_elements[3]);
    file_path = macaron::Base64::Decode(uri_elements[4]);
    full_file_path = "../../filesystem/"+username+"/"+file_path;
    timestamp_locale = uri_elements[5];

}
#include "../../include/dtos/delete_chunk_dto.hpp"

Subject DeleteChunkDTO::get_subject() const { return subject; }
int DeleteChunkDTO::getchunk_id() const { return chunk_id; }
std::string DeleteChunkDTO::getfile_name() const { return file_name; }
std::string DeleteChunkDTO::getfile_path() const { return file_path; }
int DeleteChunkDTO::gettimestamp_locale() const { return timestamp_locale; }

const void DeleteChunkDTO::fill( std::string requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    chunk_id = std::stoi(uri_elements[2]);
    file_name = std::move(uri_elements[3]);
    file_path = "../../filesystem/" + get_subject().get_sub() + "/" + file_name;
    timestamp_locale = std::stoi(uri_elements[4]);

}
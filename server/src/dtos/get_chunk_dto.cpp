#include "../../include/dtos/get_chunk_dto.hpp"

Subject GetChunkDTO::get_subject() const {return subject;}

int GetChunkDTO::getchunk_id() const { return chunk_id; }
int GetChunkDTO::getchunk_size() const { return chunk_size; }
std::string GetChunkDTO::getfile_path() const { return file_path; }
std::string GetChunkDTO::getfile_name() const { return file_name; }


const void GetChunkDTO::fill(const std::string &requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    chunk_id = std::stoi(uri_elements[2]);
    chunk_size = std::stoi(uri_elements[3]);
    file_name = uri_elements[4];
    file_path = "../../filesystem/" + get_subject().get_sub() + "/" + file_name;
}

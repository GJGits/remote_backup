#include "../../include/dtos/get_chunk_dto.hpp"

std::string GetChunkDTO::getusername() const { return username; }
int GetChunkDTO::getchunk_id() const { return chunk_id; }
int GetChunkDTO::getchunk_size() const { return chunk_size; }
std::string GetChunkDTO::getfile_path() const { return file_path; }
std::string GetChunkDTO::get_full_file_path() const { return full_file_path; };


const void GetChunkDTO::fill( std::string requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    for (unsigned int i = 0; i < uri_elements.size(); i++) {
        switch (i) {
            case 2 :
                username = uri_elements[i];
                break;
            case 3 :
                chunk_id = std::stoi(uri_elements[i]);
                break;
            case 4 :
                chunk_size = std::stoi(uri_elements[i]);

                break;
            case 5 :
                file_path = macaron::Base64::Decode(uri_elements[i]);
                full_file_path = "../../filesystem/"+username+"/"+file_path;
                break;

            default :
                break;
        }
    }
}

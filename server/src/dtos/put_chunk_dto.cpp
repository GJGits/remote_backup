#include "../../include/dtos/put_chunk_dto.hpp"

std::string PutChunkDTO::getusername() const { return username; }
int PutChunkDTO::getchunk_id() const { return chunk_id; }
int PutChunkDTO::getchunk_size() const { return chunk_size; }
std::string PutChunkDTO::getchunk_hash() const { return chunk_hash; }
std::string PutChunkDTO::getfile_path() const { return file_path; }
std::string PutChunkDTO::get_full_file_path() const { return full_file_path; }
std::vector<char> PutChunkDTO::getchunk_body() const { return chunk_body; }
std::string PutChunkDTO::gettimestamp_locale() const { return timestamp_locale; }



const void PutChunkDTO::fill( std::string requri, std::vector<char> reqbody) { //todo: passarlo per riferimento, non per copia
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
            case 5:
                chunk_hash = uri_elements[i];
                break;
            case 6 :
                file_path = macaron::Base64::Decode(uri_elements[i]);
                full_file_path = "../../filesystem/"+username+"/"+file_path;
                break;
            case 7 :
                timestamp_locale = uri_elements[i];
                break;
            default :
                break;
        }
    }
    chunk_body = reqbody;
}
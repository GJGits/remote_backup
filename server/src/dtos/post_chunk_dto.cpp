#include "../../include/dtos/post_chunk_dto.hpp"

std::string PostChunkDTO::getusername() const { return username; }
int PostChunkDTO::getchunk_id() const { return chunk_id; }
int PostChunkDTO::getchunk_size() const { return chunk_size; }
std::string PostChunkDTO::getchunk_hash() const { return chunk_hash; }
std::string PostChunkDTO::getfile_path() const { return file_path; }
std::string PostChunkDTO::get_full_file_path() const { return full_file_path; }
std::string PostChunkDTO::getnumber_of_chunks() const { return number_of_chunks; }
std::string PostChunkDTO::gettimestamp_locale() const { return timestamp_locale; }
std::vector<char> PostChunkDTO::getchunk_body() const { return chunk_body; }

const void PostChunkDTO::fill(std::string requri,std::vector<char> reqbody) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    for (unsigned int i = 0; i < uri_elements.size(); i++) {
        switch (i) {
            case 2:
                username = uri_elements[i];
                break;
            case 3:
                chunk_id = std::stoi(uri_elements[i]);
                break;
            case 4:
                chunk_size = std::stoi(uri_elements[i]);
                break;
            case 5:
                chunk_hash = uri_elements[i];
                break;
            case 6:
                number_of_chunks = uri_elements[i];
                break;
            case 7:
                file_path = macaron::Base64::Decode(uri_elements[i]);
                full_file_path = "../../filesystem/"+username+"/"+file_path;
                break;
            case 8:
                timestamp_locale = uri_elements[i];
                break;
            default:
                break;
        }
    }
    chunk_body = reqbody;
}
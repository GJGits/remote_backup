#include "../../include/dtos/delete_file_dto.hpp"

std::string DeleteFileDTO::getusername() const { return username; }
std::string DeleteFileDTO::getfile_path() const { return file_path; }
std::string DeleteFileDTO::get_full_file_path() const { return full_file_path; }

const void DeleteFileDTO::fill( const http::server::request &req) { //todo: passarlo per riferimento, non per copia
    std::vector<std::string> uri_elements = Utility::split(req.uri, '/');
    username = uri_elements[2];
    file_path = macaron::Base64::Decode(uri_elements[3]);
    full_file_path = "../../filesystem/"+username+"/"+file_path;
}
#include "../../include/dtos/delete_file_dto.hpp"

std::string DeleteFileDTO::getusername() const { return username; }
std::string DeleteFileDTO::getfile_path() const { return file_path; }
std::string DeleteFileDTO::get_full_file_path() const { return full_file_path; }

const void DeleteFileDTO::fill( std::string requri, std::vector<char> reqbody) { //todo: passarlo per riferimento, non per copia
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    for (unsigned int i = 0; i < uri_elements.size(); i++) {
        switch (i) {
            case 2 :
                username = uri_elements[i];
                break;
            case 3 :
                file_path = macaron::Base64::Decode(uri_elements[i]);
                full_file_path = "../../filesystem/"+username+"/"+file_path;
                break;
            default :
                break;
        }
    }
}
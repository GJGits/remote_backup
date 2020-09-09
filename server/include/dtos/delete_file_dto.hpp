#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"


class DeleteFileDTO{
private:
    std::string username;
    std::string file_path;

    //DELETE /file/{username}/{file_pathBASE64}
public:
    DeleteFileDTO(){};
    std::string getusername() const;
    std::string getfile_path() const;

    const void fill( std::string requri, std::vector<char> reqbody) { //todo: passarlo per riferimento, non per copia
        std::vector<std::string> uri_elements = Utility::split(requri, '/');
        for (unsigned int i = 0; i < uri_elements.size(); i++) {
            switch (i) {
                case 2 :
                    username = uri_elements[i];
                    break;
                case 3 :
                    file_path = macaron::Base64::Decode(uri_elements[i]);
                    break;
                default :
                    break;
            }
        }
    }
};
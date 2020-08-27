#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"


class GetFileDTO{
private:
    std::string username;
    std::string chunk_id;
    std::string chunk_size;
    std::string file_path;

public:
    GetFileDTO(){};
    std::string getusername() const;
    std::string getchunk_id() const;
    std::string getchunk_size() const;
    std::string getfile_path() const;

    const void fill( std::string requri) {
        std::vector<std::string> uri_elements = Utility::split(requri, '/');
        for (unsigned int i = 0; i < uri_elements.size(); i++) {
            switch (i) {
                case 2 :
                    username = uri_elements[i];
                    break;
                case 3 :
                    chunk_id = uri_elements[i];
                    break;
                case 4 :
                    chunk_size = uri_elements[i];
                    break;
                case 5 :
                    file_path = macaron::Base64::Decode(uri_elements[i]);
                    break;

                default :
                    break;
            }
        }
    }
};
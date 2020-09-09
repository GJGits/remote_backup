#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"


class GetChunkDTO{
private:
    std::string username;
    int chunk_id;
    int chunk_size;
    std::string file_path;

public:
    GetChunkDTO(){};
    std::string getusername() const;
    int getchunk_id() const;
    int getchunk_size() const;

    std::string getfile_path() const;

    const void fill( std::string requri) {
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
                    break;

                default :
                    break;
            }
        }
    }
};
#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"


class PostChunkDTO{
private:
    std::string username;
    std::string chunk_id;
    std::string chunk_size;
    std::string file_path;
    std::string chunk_hash;
    std::string timestamp_locale;
    std::vector<char> chunk_body;
//POST /chunk/{username}/{chunk_id}/{chunk_size}/{chunk_hash}/{file_pathBASE64}/{timestamp_locale}
public:
    PostChunkDTO(){};
    std::string getusername() const;
    std::string getchunk_id() const;
    std::string getchunk_size() const;
    std::string getchunk_hash() const;
    std::string getfile_path() const;
    std::string gettimestamp_locale() const;
    std::vector<char> getchunk_body() const;

    const void fill( std::string requri, std::vector<char> reqbody) { //todo: passarlo per riferimento, non per copia
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
                    chunk_hash = uri_elements[i];
                    break;
                case 6 :
                    file_path = macaron::Base64::Decode(uri_elements[i]);
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
};
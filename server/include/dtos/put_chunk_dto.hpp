#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"


class PutChunkDTO{
private:
    std::string username;
    int chunk_id;
    int chunk_size;
    std::string chunk_hash;
    std::string file_path;
    std::vector<char> chunk_body;
    std::string number_of_chunks;
    std::string timestamp_locale;

    //PUT /chunk/{username}/{chunk_id}/{chunksize}/{chunk_hash}/{number_of_chunks}/{file_pathBASE64}/{timestamp}
public:
    PutChunkDTO(){};
    std::string getusername() const;
    int getchunk_id() const;
    int getchunk_size() const;
    std::string getchunk_hash() const;
    std::string getfile_path() const;
    std::vector<char> getchunk_body() const; //todo: attenzione, passare all'heap
    std::string getnumber_of_chunks() const;
    std::string gettimestamp_locale() const;


    const void fill( std::string requri, std::vector<char> reqbody) { //todo: passarlo per riferimento, non per copia
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
                    number_of_chunks = uri_elements[i];
                    break;
                case 7 :
                    file_path = macaron::Base64::Decode(uri_elements[i]);
                    break;
                case 8 :
                    timestamp_locale = uri_elements[i];
                    break;


                default :
                    break;
            }
        }
        chunk_body = reqbody;
    }
};
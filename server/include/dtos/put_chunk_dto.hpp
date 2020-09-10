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
    std::string full_file_path;
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
    std::string get_full_file_path() const;
    std::vector<char> getchunk_body() const;
    std::string getnumber_of_chunks() const;
    std::string gettimestamp_locale() const;
    const void fill( std::string requri, std::vector<char> reqbody);
};
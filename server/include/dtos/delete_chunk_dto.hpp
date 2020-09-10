#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"


class DeleteChunkDTO{
private:
    std::string username;
    int chunk_id;
    int chunk_size;
    std::string file_path;
    std::string full_file_path;
    std::string timestamp_locale;

public:
    DeleteChunkDTO(){};
    std::string getusername() const;
    int getchunk_id() const;
    int getchunk_size() const;
    std::string getfile_path() const;
    std::string get_full_file_path() const;
    std::string gettimestamp_locale() const;

    const void fill( std::string requri);
};
#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"


class DeleteChunkDTO{
private:
    std::string username;
    int chunk_id;
    std::string file_path;
    int timestamp_locale;

public:
    DeleteChunkDTO(){};
    std::string getusername() const;
    int getchunk_id() const;
    std::string getfile_path() const;
    int gettimestamp_locale() const;

    const void fill( std::string requri);
};
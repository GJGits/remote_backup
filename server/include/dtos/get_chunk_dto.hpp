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
    std::string full_file_path;


public:
    GetChunkDTO(){};
    std::string getusername() const;
    int getchunk_id() const;
    int getchunk_size() const;
    std::string getfile_path() const;
    std::string get_full_file_path() const;
    const void fill(const std::string &requri);
};
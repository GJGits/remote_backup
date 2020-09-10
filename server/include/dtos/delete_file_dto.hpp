#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"


class DeleteFileDTO{
private:
    std::string username;
    std::string file_path;
    std::string full_file_path;

public:
    DeleteFileDTO(){};
    std::string getusername() const;
    std::string getfile_path() const;
    std::string get_full_file_path() const;

    const void fill( std::string requri, std::vector<char> reqbody);
};
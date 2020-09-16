#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"
#include "../http/request.hpp"


class DeleteFileDTO{
private:
    std::string username;
    std::string file_path;

public:
    DeleteFileDTO(){};
    std::string getusername() const;
    std::string getfile_path() const;

    const void fill( const http::server::request &req);
};
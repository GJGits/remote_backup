#pragma once

#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "../http/request.hpp"
#include "json-serializable.hpp"
#include "subject.hpp"
#include <iostream>

class PutChunkDTO {
private:
    Subject subject;
    std::string old_path_64;
    std::string old_file_path_64;
    std::string new_path_64;
    std::string new_file_path_64;

public:
    PutChunkDTO(const Subject &subject) : subject{subject} {}
    Subject get_subject() const;
    std::string getold_path_64() const;
    std::string getold_file_path_64() const;
    std::string getnew_path_64() const;
    std::string getnew_file_path_64() const;
    const void fill(const http::server::request &req);
};
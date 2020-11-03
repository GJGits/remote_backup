#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"
#include "../http/request.hpp"
#include "subject.hpp"


class DeleteFileDTO{
private:
    Subject subject;
    std::string file_path;
    int timestamp_locale;

public:
    DeleteFileDTO(){}
    DeleteFileDTO(const Subject &subject) : subject{subject} {}
    Subject get_subject() const;
    std::string getfile_path() const;
    int gettimestamp_locale() const;

    const void fill( const http::server::request &req);
};
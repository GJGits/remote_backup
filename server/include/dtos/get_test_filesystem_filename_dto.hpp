#pragma once
#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "json-serializable.hpp"
#include "subject.hpp"

class GetTestFilesystemFilenameDTO {
private:
    std::string file_name;
    std::string username;
    int page_num;
public:
    GetTestFilesystemFilenameDTO() {}
    std::string get_file_name() const;
    std::string get_username() const;
    int getpage_num() const;
    const void fill(const std::string &requri);
};
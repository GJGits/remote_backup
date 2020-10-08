#pragma once
#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "json-serializable.hpp"
#include "subject.hpp"

class GetTestFilesystemFilenameDTO {
private:
    Subject subject;
    std::string file_name;
    int page_num;
public:
    GetTestFilesystemFilenameDTO(const Subject &subject) : subject{subject} {}
    Subject get_subject() const;
    std::string get_file_name() const;
    int getpage_num() const;
    const void fill(const std::string &requri);
};
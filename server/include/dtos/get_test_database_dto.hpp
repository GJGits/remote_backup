#pragma once
#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "json-serializable.hpp"
#include "subject.hpp"

class GetTestDatabaseDTO {
private:
    Subject subject;
    int page_num;
    std::string table_name;
public:
    GetTestDatabaseDTO(const Subject &subject) : subject{subject} {}
    Subject get_subject() const;
    int getpage_num() const;
    std::string get_table_name() const;
    const void fill(const std::string &requri);
};
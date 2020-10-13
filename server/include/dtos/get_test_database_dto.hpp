#pragma once
#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "json-serializable.hpp"
#include "subject.hpp"

class GetTestDatabaseDTO {
private:
    int page_num;
    int db_num;
    std::string table_name;
public:
    GetTestDatabaseDTO() {}
    int getpage_num() const;
    int getdb_num() const;
    std::string get_table_name() const;
    const void fill(const std::string &requri);
};
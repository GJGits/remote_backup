#include "../../include/dtos/get_test_database_dto.hpp"


Subject GetTestDatabaseDTO::get_subject() const {return subject;}

const void GetTestDatabaseDTO::fill(const std::string &requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    table_name = std::move(uri_elements[3]);
    page_num = std::stoi(uri_elements[4]);
}

int GetTestDatabaseDTO::getpage_num() const { return page_num; }
std::string GetTestDatabaseDTO::get_table_name() const {return table_name;}
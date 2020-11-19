#include "../../include/dtos/get_test_database_dto.hpp"



const void GetTestDatabaseDTO::fill(const std::string &requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    table_name = std::move(uri_elements[3]);
    db_num = std::stoi(uri_elements[4]);
    page_num = std::stoi(uri_elements[5]);
}

int GetTestDatabaseDTO::getpage_num() const { return page_num; }
int GetTestDatabaseDTO::getdb_num() const { return db_num; }
std::string GetTestDatabaseDTO::get_table_name() const {return table_name;}
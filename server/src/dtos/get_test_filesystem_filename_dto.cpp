#include "../../include/dtos/get_test_filesystem_filename_dto.hpp"



const void GetTestFilesystemFilenameDTO::fill(const std::string &requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    username = std::move(uri_elements[3]);
    file_name = std::move(uri_elements[4]);
    page_num = std::stoi(uri_elements[5]);

}

std::string GetTestFilesystemFilenameDTO::get_file_name() const {return file_name;}
std::string GetTestFilesystemFilenameDTO::get_username() const {return username;}
int GetTestFilesystemFilenameDTO::getpage_num() const { return page_num; }
#include "../../include/dtos/get_test_filesystem_filename_dto.hpp"


Subject GetTestFilesystemFilenameDTO::get_subject() const {return subject;}

const void GetTestFilesystemFilenameDTO::fill(const std::string &requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    file_name = std::move(uri_elements[3]);
    page_num = std::stoi(uri_elements[4]);

}

std::string GetTestFilesystemFilenameDTO::get_file_name() const {return file_name;}

int GetTestFilesystemFilenameDTO::getpage_num() const { return page_num; }
#include "../../include/dtos/get_status_dto.hpp"

Subject GetStatusDTO::get_subject() const {return subject;}

const void GetStatusDTO::fill(const std::string &requri) {
    std::vector<std::string> uri_elements = Utility::split(requri, '/');
    page_num = std::stoi(uri_elements[3]);
}

int GetStatusDTO::getpage_num() const { return page_num; }

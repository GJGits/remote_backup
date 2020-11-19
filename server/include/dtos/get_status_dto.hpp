#pragma once
#include "../common/utility.hpp"
#include "subject.hpp"

class GetStatusDTO {
private:
    Subject subject;
    int page_num;
    int last_check;
public:
    GetStatusDTO(const Subject &subject) : subject{subject} {}
    Subject get_subject() const;
    int getpage_num() const;
    int get_last_check() const;
    const void fill(const std::string &requri);
};
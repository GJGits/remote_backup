#pragma once
#include "../common/base64.hpp"
#include "../common/utility.hpp"
#include "json-serializable.hpp"
#include "subject.hpp"

class GetStatusDTO {
private:
    Subject subject;
    int page_num;

public:
    GetStatusDTO(const Subject &subject) : subject{subject} {}
    Subject get_subject() const;
    int getpage_num() const;
    const void fill(const std::string &requri);
};
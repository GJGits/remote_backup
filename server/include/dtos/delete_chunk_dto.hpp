#pragma once
#include "json-serializable.hpp"
#include "../common/utility.hpp"
#include "../common/base64.hpp"
#include "subject.hpp"


class DeleteChunkDTO{
private:
    Subject subject;
    std::string file_name;
    int chunk_id;
    std::string file_path;
    int timestamp_locale;

public:
    DeleteChunkDTO(){}
    DeleteChunkDTO(const Subject &subject):subject{subject} {}
    Subject get_subject() const;
    int getchunk_id() const;
    std::string getfile_name() const;
    std::string getfile_path() const;
    int gettimestamp_locale() const;

    const void fill( std::string requri);
};
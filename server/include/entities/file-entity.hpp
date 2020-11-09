#pragma once
#include <string>
#include "../dtos/delete_file_dto.hpp"

class FileEntity {
private:
    std::string path_file;
    int last_mod;
    Subject subject;

public:
    FileEntity(const DeleteFileDTO &dto);
    std::string getPathFile() const;
    int getLastMod() const;
    Subject get_subject() const;
};
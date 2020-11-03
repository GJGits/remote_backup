#pragma once
#include <string>
#include "../common/logger.hpp"
#include "../dtos/delete_file_dto.hpp"

class FileEntity {
private:
    std::string username;
    std::string path_file;
    int last_mod;
    Subject subject;

public:
    FileEntity(std::string username, std::string path_file, int last_mod);
    std::string getUsername() const;
    std::string getPathFile() const;
    int getLastMod() const;
    Subject get_subject() const;
};
#pragma once
#include <string>
#include "../common/logger.hpp"
#include "../dtos/delete_file_dto.hpp"

class FileEntity {
private:
    std::string username;
    std::string path_file;
    Subject subject;

public:
    FileEntity(std::string username, std::string path_file);
    std::string getUsername() const;
    std::string getPathFile() const;
    Subject get_subject() const;
};
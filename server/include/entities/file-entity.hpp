#pragma once
#include <string>
#include "../common/logger.hpp"

class FileEntity {
private:
    std::string username;
    std::string path_file;

public:
    FileEntity(std::string username, std::string path_file);
    std::string getUsername() const;
    std::string getPathFile() const;
};
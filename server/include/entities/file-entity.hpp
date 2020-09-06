#pragma once
#include <string>
#include "../common/logger.hpp"

class FileEntity {
private:
    std::string username;
    std::string path_file;

public:
    FileEntity(std::string username, std::string path_file):
            username{std::move(username)}, path_file{std::move(path_file)}{};

    std::string getUsername() const{ return username; };
    std::string getPathFile() const{return path_file;};

};
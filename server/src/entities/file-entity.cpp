#include "../../include/entities/file-entity.hpp"

FileEntity::FileEntity(std::string username, std::string path_file):
username{std::move(username)}, path_file{std::move(path_file)}{};


std::string FileEntity::getUsername() const{ return username; };
std::string FileEntity::getPathFile() const{return path_file;};
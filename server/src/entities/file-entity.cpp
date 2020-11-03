#include "../../include/entities/file-entity.hpp"

FileEntity::FileEntity(std::string username, std::string path_file, int last_mod):
username{std::move(username)}, path_file{std::move(path_file)}, last_mod{last_mod}{};


std::string FileEntity::getUsername() const{ return username; };
std::string FileEntity::getPathFile() const{return path_file;};
int FileEntity::getLastMod() const { return last_mod; };

Subject FileEntity::get_subject() const {return subject;}
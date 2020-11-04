#include "../../include/entities/file-entity.hpp"

FileEntity::FileEntity(const DeleteFileDTO &dto):
path_file{dto.getfile_path()}, last_mod{dto.gettimestamp_locale()}, subject{dto.get_subject()}{};

std::string FileEntity::getPathFile() const{return path_file;};
int FileEntity::getLastMod() const { return last_mod; };

Subject FileEntity::get_subject() const {return subject;}
#include "../../include/repositories/file-repository.hpp"

std::shared_ptr<FileRepository> FileRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<FileRepository>{};
  }
  return instance;
}

bool FileRepository::deleteFile(const FileEntity &file) {

    std::string query = "UPDATE chunks SET device_id = 0, c_lastmod = "+std::to_string(file.getLastMod())+" WHERE c_username = '?' and c_path = '?';";
    std::list<std::string> entries_of_query;
    entries_of_query.push_back(file.get_subject().get_sub());
    entries_of_query.push_back(file.getPathFile());

    return CommonRepository::update_query(query,entries_of_query,file.get_subject().get_db_id());

}
#include "../../include/repositories/file-repository.hpp"

std::shared_ptr<FileRepository> FileRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<FileRepository>{};
  }
  return instance;
}

bool FileRepository::deleteFile(const FileEntity &file) {

    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t db_selected = db_repinstance->getDBbyUsername(file.getUsername());

    std::string query = "DELETE from chunks WHERE c_username = '?' and c_path = '?';";
    std::list<std::string> entries_of_query;
    entries_of_query.push_back(file.getUsername());
    entries_of_query.push_back(file.getPathFile());

    return CommonRepository::update_query(query,entries_of_query,db_selected);

}
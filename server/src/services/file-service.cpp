#include "../../include/services/file-service.hpp"

std::shared_ptr<FileService> FileService::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<FileService>(new FileService{});
    instance->file_repository = FileRepository::getInstance();
    instance->user_repository = UserRepository::getInstance();
  }
  return instance;
}

std::string FileService::getStatusFile(const std::string &username) {
  user_repository->getUserByUsername(username);
  json status_file = user_repository->get_status_file(username);
  status_file["hashed_status"] = user_repository->get_hashed_status(username);
  return status_file.dump();
}

void FileService::delete_file_service(const DeleteFileDTO &del_file) {

  FileEntity file_ent{del_file.get_subject().get_sub(),
                      del_file.getfile_path()};
  std::filesystem::remove_all(del_file.getfile_path());
  file_repository->deleteFile(file_ent);
}
#include "../../include/services/file-service.hpp"

std::shared_ptr<FileService> FileService::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<FileService>(new FileService{});
    instance->file_repository = FileRepository::getInstance();
    instance->user_repository = UserRepository::getInstance();
  }
  return instance;
}

std::string FileService::getStatusFile(const GetStatusDTO &get_status_dto) {
  UserEntity user_entity{get_status_dto.get_subject().get_sub(), get_status_dto.getpage_num()};
  //user_repository->getUserByUsername(username);
  json status_file = user_repository->get_status_file(user_entity);
  //status_file["hashed_status"] = user_repository->get_hashed_status(username);
  std::clog << status_file.dump() << "\n";
  return status_file.dump();
}

void FileService::delete_file_service(const DeleteFileDTO &del_file) {

  FileEntity file_ent{del_file.get_subject().get_sub(),
                      del_file.getfile_path()};
  std::filesystem::remove_all(del_file.getfile_path());
  file_repository->deleteFile(file_ent);
}
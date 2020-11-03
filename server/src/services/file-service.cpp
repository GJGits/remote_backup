#include "../../include/services/file-service.hpp"

std::shared_ptr<FileService> FileService::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<FileService>(new FileService{});
    instance->file_repository = FileRepository::getInstance();
    instance->user_repository = UserRepository::getInstance();
  }
  return instance;
}

const json FileService::getStatusFile(const GetStatusDTO &get_status_dto) {
  return user_repository->get_status_file(get_status_dto.get_subject() , get_status_dto.getpage_num(), get_status_dto.get_last_check());
}

void FileService::delete_file_service(const DeleteFileDTO &del_file) {

  FileEntity file_ent{del_file.get_subject().get_sub(),
                      del_file.getfile_path()};
  std::filesystem::remove_all("../../filesystem/"+del_file.get_subject().get_sub()+"/"+del_file.getfile_path());
  file_repository->deleteFile(file_ent);
}
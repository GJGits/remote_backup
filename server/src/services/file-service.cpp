#include "../../include/services/file-service.hpp"



const json FileService::getStatusFile(const GetStatusDTO &get_status_dto) {
    std::shared_ptr<UserRepository> user_repository = UserRepository::getInstance();
    return user_repository->get_status_file(get_status_dto.get_subject() , get_status_dto.getpage_num(), get_status_dto.get_last_check());
}

void FileService::delete_file_service(const DeleteFileDTO &del_file) {
    std::clog << "entro nella delete_file_service\n";
  FileEntity file_ent{del_file};
  std::filesystem::remove_all("../../filesystem/"+del_file.get_subject().get_sub()+"/"+del_file.getfile_path());
    std::clog << "ho rimosso: ../../filesystem/"+del_file.get_subject().get_sub()+"/"+del_file.getfile_path()<< "\n";

    std::shared_ptr<FileRepository> file_repository = FileRepository::getInstance();
    file_repository->deleteFile(file_ent);
    std::clog << "Sono dopo la cancellazione \n";

}
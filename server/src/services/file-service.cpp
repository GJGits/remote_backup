#include "../../include/services/file-service.hpp"

std::shared_ptr<FileService> FileService::getInstance() {
    if (instance.get() == nullptr) {
        instance = std::shared_ptr<FileService>(new FileService{});
    }
    return instance;
}


std::string FileService::getStatusFile(const std::string &username) {
    UserRepository user_rep;
    user_rep.getUserByUsername(username);
    json status_file = user_rep.get_status_file(username);
    status_file["hashed_status"] = user_rep.get_hashed_status(username);
    return status_file.dump();
}



void FileService::delete_file_service(const DeleteFileDTO &del_file) {

    FileEntity file_ent{del_file.get_subject().get_sub(),del_file.getfile_path()};
    std::filesystem::remove_all(del_file.getfile_path());
    FileRepository file_rep;
    file_rep.deleteFile(file_ent);
}
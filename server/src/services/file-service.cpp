#include "../../include/services/file-service.hpp"



std::string FileService::getStatusFile(const std::string &username) {
    UserRepository user_rep;
    user_rep.getUserByUsername(username);
    json status_file = user_rep.get_status_file(username);
    status_file["hashed_status"] = user_rep.get_hashed_status(username);
    return status_file.dump();
}



void FileService::delete_file_service(const DeleteFileDTO &del_file) {
    FileEntity file_ent{del_file.getusername(),del_file.getfile_path()};
    FileRepository file_rep;
    if(file_rep.deleteFile(file_ent)){
        remove(("../../filesystem/" + del_file.getusername() + "/" + del_file.getfile_path()).c_str());
        return;
    }
    throw FileNotDeleted();
}
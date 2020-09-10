#include "../../include/services/user-service.hpp"



std::string UserService::login(const SigninDTO &user) {
  UserRepository user_rep;
  UserEntity user_returned =
      user_rep.getUserByUsername(user.getUsername());
    unsigned int salt = user_returned.getSalt();
    std::string password_to_hash{std::to_string(salt) + user.getPassword() + std::to_string(salt)};
    std::vector<char> vec(password_to_hash.begin(), password_to_hash.end());
    std::string hashed_password{Sha256::getSha256(vec)};
    if (user_returned.getHashedPassword().compare(hashed_password) == 0)
      return JWT::generateToken(user.getUsername(), JWT::getExpiration() + std::time(nullptr));
    else
      throw CredentialsNotValidException();
}

std::string UserService::signup(const SignupDTO &user) {
    if (user.getPassword().compare(user.getPasswordConfirm()) != 0)
        throw PasswordNeqConfirm();
    UserRepository user_rep;
    std::string username(user.getUsername());
    unsigned int salt = abs(rand());
    std::string password_to_hash{std::to_string(salt) + user.getPassword() + std::to_string(salt)};
    std::vector<char> vec(password_to_hash.begin(), password_to_hash.end());
    std::string hashedPassword{Sha256::getSha256(vec)};
    UserEntity user_to_insert{username, hashedPassword, salt};
    if (user_rep.insertUser(user_to_insert)) {
        std::string path{"../../filesystem/" + username};
        std::filesystem::create_directories(path);
        return JWT::generateToken(user.getUsername(), JWT::getExpiration() + std::time(nullptr));
    }
    throw UsernameAlreadyExists();
}

std::string UserService::getStatus(const std::string &username) {
  UserRepository user_rep;
  user_rep.getUserByUsername(username);
  return user_rep.get_hashed_status(username);

}

std::string UserService::getStatusFile(const std::string &username) {
    UserRepository user_rep;
    user_rep.getUserByUsername(username);
    json status_file = user_rep.get_status_file(username);
    status_file["hashed_status"] = user_rep.get_hashed_status(username);
    return status_file.dump();
}










void UserService::delete_file_service(const DeleteFileDTO &del_file) {
    FileEntity file_ent{del_file.getusername(),del_file.getfile_path()};
    FileRepository file_rep;
    if(file_rep.deleteFile(file_ent)){
        remove(("../../filesystem/" + del_file.getusername() + "/" + del_file.getfile_path()).c_str());
        return;
    }
    throw FileNotDeleted();
}






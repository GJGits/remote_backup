#include "../../include/services/user-service.hpp"

std::string UserService::login(const SigninDTO &user) {
  UserRepository user_rep;
  UserEntity user_returned =
      user_rep.getUserByUsername(user.getUsername());

    unsigned int salt = user_returned.getSalt();
    std::string hashed_password{Sha256::getSha256(
        std::to_string(salt) + user.getPassword() + std::to_string(salt))};
    if (user_returned.getHashedPassword().compare(hashed_password) == 0)
      return JWT::generateToken(user.getUsername(), JWT::getExpiration() + std::time(nullptr));
    else
      throw CredentialsNotValidException();


  throw UknownError();
}

std::string UserService::signup(const SignupDTO &user) {

  /* Eventualmente tale check se deve essere fatto più avanti lo si sposta*/
  if (user.getPassword().compare(user.getPasswordConfirm()) != 0)
    throw PasswordNeqConfirm();

  UserRepository user_rep;
  std::string username(user.getUsername());
  unsigned int salt = abs(rand());
  std::string hashedPassword{Sha256::getSha256(
      std::to_string(salt) + user.getPassword() + std::to_string(salt))};
  UserEntity user_to_insert{username, hashedPassword, salt};
  if (user_rep.insertUser(user_to_insert)) {
    /* Create user directory */
    std::string path{"../../filesystem/" + username};
    std::filesystem::create_directories(path);

    /* Create client-struct.json file */
    json j ;
    j["hashed_status"] = "empty_hashed_status";
    j["entries"] = json::array();
    std::ofstream o(path + "/client-struct.json");
    o << std::setw(4) << j << std::endl;
    return JWT::generateToken(user.getUsername(), JWT::getExpiration() + std::time(nullptr));
  } else
    throw UsernameAlreadyExists();
}

std::string UserService::getStatus(const std::string &username) {
  UserRepository user_rep;
  user_rep.getUserByUsername(username);
    json j;
    std::ifstream i("../../filesystem/" + username + "/client-struct.json");
    i >> j;
    return j["hashed_status"];

}
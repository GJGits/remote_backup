#include "../../include/services/user-service.hpp"


std::string UserService::login(const UserLogDTO &user) {
  UserRepository user_rep;
  std::optional<UserEntity> user_returned =
      user_rep.getUserByUsername(user.getUsername());
  if (user_returned.has_value()) {
    unsigned int salt = user_returned.value().getSalt();
    std::string hashed_password{Sha256::getSha256(
        std::to_string(salt) + user.getPassword() + std::to_string(salt))};
    if (user_returned.value().getHashedPassword().compare(hashed_password) == 0)
      return JWT::generateToken(user.getUsername());
    else
      throw CredentialsNotValidException();
  } else
    throw UsernameNotExists();

  throw UknownError();
}

std::string UserService::signup(const UserLogDTO &user) {

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
    std::string file_path{path + "/client-struct.json"};
    std::ofstream init_file;
    std::string content{
        "{\n \"hashed_status\":\"empty_hashed_status\",\n \"entries\":[]\n}"};
    init_file.open(file_path);
    init_file << content;
    init_file.close();
      return JWT::generateToken(user.getUsername());
  } else
      throw UsernameAlreadyExists();
}

std::string
UserService::getStatus(const UserLogDTO &user) {
  UserRepository user_rep;
  std::optional<UserEntity> user_returned =
      user_rep.getUserByUsername(user.getUsername());
  /* Se viene fornito un username non esistente nel database, torna stringa
   * vuota, che poi verrà tornata come bad::request */
  if (user_returned.has_value()) {
    std::string file_path{"../../filesystem/" + user.getUsername() +
                          "/client-struct.json"};
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(file_path, pt);
    return pt.get<std::string>("hashed_status");
  } else {
    throw UsernameNotExists();
  }
}
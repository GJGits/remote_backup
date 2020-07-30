
#include "../../include/services/user-service.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "../../include/error_message/error-message.hpp"

std::tuple<error_enum,std::string> UserService::login(const UserLogDTO &user) {
    Error_message *err = Error_message::getInstance();
  UserRepository user_rep;
  std::optional<UserEntity> user_returned =
      user_rep.getUserByUsername(user.getUsername());
  if (user_returned.has_value()) {
    unsigned int salt = user_returned.value().getSalt();
    std::string hashed_password{Sha256::getSha256(
        std::to_string(salt) + user.getPassword() + std::to_string(salt))};
    if (user_returned.value().getHashedPassword().compare(hashed_password) == 0)
      return std::make_tuple(ok,err->get_error_message(ok));
    else
      return std::make_tuple(wrong_credentials,err->get_error_message(wrong_credentials));
  } else
    return std::make_tuple(username_not_exist,err->get_error_message(username_not_exist));

  return std::make_tuple(generic_error,err->get_error_message(generic_error));
}

std::tuple<error_enum,std::string> UserService::signup(const UserLogDTO &user) {
    Error_message *err = Error_message::getInstance();

  /* Eventualmente tale check se deve essere fatto più avanti lo si sposta*/
  if (user.getPassword().compare(user.getPasswordConfirm()) != 0)
    return std::make_tuple(pass_passconfirm_neq,err->get_error_message(pass_passconfirm_neq));

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

    /* Create server-struct.json file */
    std::string file_path{path + "/server-struct.json"};
    std::ofstream init_file;
    std::string content{
        "{\n \"hashed_status\":\"empty_hashed_status\",\n \"entries\":[]\n}"};
    init_file.open(file_path);
    init_file << content;
    init_file.close();
    return std::make_tuple(ok,err->get_error_message(ok));
  } else
    return std::make_tuple(user_already_exist,err->get_error_message(user_already_exist));
}


std::tuple<error_enum,std::string> UserService::getStatus(const UserLogDTO &user) {
    Error_message *err = Error_message::getInstance();
    UserRepository user_rep;
  std::optional<UserEntity> user_returned =
      user_rep.getUserByUsername(user.getUsername());
  /* Se viene fornito un username non esistente nel database, torna stringa
   * vuota, che poi verrà tornata come bad::request */
  if (user_returned.has_value()) {
    std::string file_path{"../../filesystem/" + user.getUsername() +
                          "/server-struct.json"};
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(file_path, pt);
    return std::make_tuple(ok,pt.get<std::string>("hashed_status"));
  } else {
    return std::make_tuple(username_not_exist,err->get_error_message(username_not_exist));
  }
}
#include "../../include/services/user-service.hpp"

std::string UserService::login(const SigninDTO &user) {
  std::shared_ptr<UserRepository> user_repository =
      UserRepository::getInstance();
  UserEntity user_returned =
      user_repository->getUserByUsername(user.getUsername());
  unsigned int salt = user_returned.getSalt();
  std::string password_to_hash{std::to_string(salt) + user.getPassword() +
                               std::to_string(salt)};
  std::vector<char> vec(password_to_hash.begin(), password_to_hash.end());
  std::string hashed_password{Sha256::getSha256(vec)};
  if (user_returned.getHashedPassword().compare(hashed_password) == 0) {
    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t db_sel = db_repinstance->getDBbyUsername(user.getUsername());
    int device_id = user_returned.check_validity_id(user.getMAC());
    if (device_id < 0) {
      user_repository->updateUser(user_returned);
      device_id = abs(device_id);
    }
    Subject sub{user.getUsername(), db_sel, (size_t)device_id};

    return JWT::generateToken(sub, JWT::getExpiration() + std::time(nullptr));
  } else
    throw CredentialsNotValidException();
}

std::string UserService::signup(const SignupDTO &user) {

  if (user.getPassword().compare(user.getPasswordConfirm()) != 0)
    throw WrongRquestFormat();

  std::string username(user.getUsername());
  std::shared_ptr<UserRepository> user_repository =
      UserRepository::getInstance();
  if (user_repository->UserAlreadyPresent(user.getUsername()))
    throw UsernameAlreadyExists();

  unsigned int salt = abs(rand());
  std::string password_to_hash{std::to_string(salt) + user.getPassword() +
                               std::to_string(salt)};
  std::vector<char> vec(password_to_hash.begin(), password_to_hash.end());
  std::string hashedPassword{Sha256::getSha256(vec)};
  UserEntity user_to_insert{username, hashedPassword, salt};
  user_to_insert.set_device_1_MAC(user.getMAC());
  size_t db_sel = user_repository->insertUser(user_to_insert);
  Subject sub{user.getUsername(), db_sel, 1};
  return JWT::generateToken(sub, JWT::getExpiration() + std::time(nullptr));
}

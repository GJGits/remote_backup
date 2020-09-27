#include "../../include/services/user-service.hpp"

std::shared_ptr<UserService> UserService::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<UserService>(new UserService{});
    instance->user_repository = UserRepository::getInstance();
  }
  return instance;
}

std::string UserService::login(const SigninDTO &user) {
  UserEntity user_entity{user.getUsername(), user.getMAC()};
  UserEntity user_returned = user_repository->getUserByUsername(user_entity);
  unsigned int salt = user_returned.getSalt();
  std::string password_to_hash{std::to_string(salt) + user.getPassword() +
                               std::to_string(salt)};
  std::vector<char> vec(password_to_hash.begin(), password_to_hash.end());
  std::string hashed_password{Sha256::getSha256(vec)};
  if (user_returned.getHashedPassword().compare(hashed_password) == 0) {
    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t db_sel = db_repinstance->getDBbyUsername(user.getUsername());
    size_t device_id = user_repository->getdevicebyUsernameandMac(user_entity); // todo: selezionare da db utilizzando MAC
    std::clog << "Il db selezionato è:"<< device_id << "\n";
    Subject sub{user.getUsername(), db_sel, device_id};
    return JWT::generateToken(sub, JWT::getExpiration() + std::time(nullptr));
  }

  else
    throw CredentialsNotValidException();
}

std::string UserService::signup(const SignupDTO &user) {

  if (user.getPassword().compare(user.getPasswordConfirm()) != 0)
    throw PasswordNeqConfirm();

  UserEntity user_entity{user.getUsername(), user.getMAC()};

  std::string username(user.getUsername());
  unsigned int salt = abs(rand());
  std::string password_to_hash{std::to_string(salt) + user.getPassword() +
                               std::to_string(salt)};
  std::vector<char> vec(password_to_hash.begin(), password_to_hash.end());
  std::string hashedPassword{Sha256::getSha256(vec)};
  UserEntity user_to_insert{username, hashedPassword, salt};
  size_t db_sel = user_repository->insertUser(user_to_insert);
    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t device_id = user_repository->getdevicebyUsernameandMac(user_entity); // todo: selezionare da db utilizzando MAC
    std::clog << "Il db selezionato è:"<< device_id << "\n";
  Subject sub{user.getUsername(), db_sel, device_id};
  return JWT::generateToken(sub, JWT::getExpiration() + std::time(nullptr));

  throw UsernameAlreadyExists();
}

std::string UserService::getStatus(const std::string &username) {
  user_repository->getUserByUsername(username);
  return user_repository->get_hashed_status(username);
}
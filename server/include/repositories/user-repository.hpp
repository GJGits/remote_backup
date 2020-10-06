#pragma once
#include <memory>
#include <optional>
#include <cmath>
#include "../common/json.hpp"
#include "../common/constants.hpp"
#include "../common/sha256.hpp"
#include "../common/utility.hpp"
#include "../entities/db-connect.hpp"
#include "../entities/user-entity.hpp"
#include "../exceptions/exceptions.hpp"


using json = nlohmann::json;

class UserRepository {
private:
  static inline std::shared_ptr<UserRepository> instance{nullptr};

public:
  size_t insertUser(const UserEntity &user);
  UserEntity getUserByUsername(const std::string &username);
  bool deleteUserByUsername(const std::string &username);
  std::string get_hashed_status(const std::string &username);
  json get_status_file(const UserEntity &user_entity);
  void updateUser(const UserEntity &user);
  static std::shared_ptr<UserRepository> getInstance();
};
#pragma once
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/user-entity.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/sha256.hpp"
#include "../common/json.hpp"

using json = nlohmann::json;

class UserRepository {
private:
public:
  size_t insertUser(const UserEntity &user);
  UserEntity getUserByUsername(const std::string &username);
  bool deleteUserByUsername(const std::string &username);
  std::string get_hashed_status(const std::string &username);
  json get_status_file(const std::string &username);
};
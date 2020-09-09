#pragma once
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/user-entity.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/sha256.hpp"

class UserRepository {
private:
public:
  bool insertUser(const UserEntity &user);
  UserEntity getUserByUsername(const std::string &username);
  bool deleteUserByUsername(const std::string &username);
  std::string update_hashed_status(const std::string &username);
};
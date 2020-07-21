#pragma once
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/user-entity.hpp"

class UserRepository {
private:
public:
  bool insertUser(const UserEntity &user);
  std::optional<UserEntity> getUserByUsername(const std::string &username);
  bool deleteUserByUsername(const std::string &username);
};
#pragma once
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/user-entity.hpp"
#include "../exceptions/exceptions.hpp"

class UserRepository {
private:
public:
  bool insertUser(const UserEntity &user);
  UserEntity getUserByUsername(const std::string &username);
  bool deleteUserByUsername(const std::string &username);
};
#pragma once
#include <string>
#include "../common/logger.hpp"

class UserEntity {
private:
  std::string username;
  std::string hashed_password;
  unsigned int salt;

public:
  UserEntity(std::string username, std::string hashed_password,
             unsigned int salt);

  std::string getUsername() const;
  std::string getHashedPassword() const;
  unsigned int getSalt() const;
};
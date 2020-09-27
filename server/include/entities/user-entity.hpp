#pragma once
#include <string>
#include "../common/logger.hpp"

class UserEntity {
private:
  std::string username;
  std::string hashed_password;
  unsigned int salt;
  std::string mac;

public:
  UserEntity(const std::string &username, const std::string &hashed_password,
             unsigned int salt);

  UserEntity(const std::string &username, const std::string &mac);

  UserEntity(const std::string &username);

  std::string getUsername() const;
  std::string getHashedPassword() const;
  unsigned int getSalt() const;
  std::string getMac() const;


};
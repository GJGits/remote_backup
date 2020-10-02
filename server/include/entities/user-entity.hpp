#pragma once
#include <string>
#include "../common/logger.hpp"

class UserEntity {
private:
  std::string username;
  std::string hashed_password;
  unsigned int salt;
  int page_num;

public:

  UserEntity(const std::string &username, const std::string &hashed_password,
             unsigned int salt);

  UserEntity(const std::string &username, int page_num);

  std::string getUsername() const;
  std::string getHashedPassword() const;
  unsigned int getSalt() const;
  int getpage_num() const;

};
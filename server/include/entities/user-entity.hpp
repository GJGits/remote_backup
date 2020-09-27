#pragma once
#include <string>
#include "../common/logger.hpp"

class UserEntity {
private:
  std::string username;
  std::string hashed_password;
  unsigned int salt;
  std::string device1;
    std::string device2;
    std::string device3;
    int numdevices;
public:
    UserEntity(const std::string &username,
               const std::string &hashed_password, uint32_t salt,
               const std::string &device1,
               const std::string &device2,
               const std::string &device3,
               int numdevices);

  UserEntity(const std::string &username, const std::string &hashed_password,
             unsigned int salt);

  UserEntity(const std::string &username,
                           const std::string &hashed_password, uint32_t salt,
                           const std::string &device1);

  UserEntity(const std::string &username);

  std::string getUsername() const;
  std::string getHashedPassword() const;
  unsigned int getSalt() const;
  std::string getDevice1() const;
std::string getDevice2() const;
std::string getDevice3() const;
    int getnumdevices() const;
    void setDevices(int actual_num_fields, std::string mac) ;

};
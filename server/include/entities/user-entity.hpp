#pragma once
#include "../common/logger.hpp"
#include "../dtos/subject.hpp"
#include "../exceptions/exceptions.hpp"
#include <string>

class UserEntity {
private:
  Subject subject;
  std::string username;
  std::string hashed_password;
  unsigned int salt;
  int page_num;
  std::string device_1_MAC;
  std::string device_2_MAC;
  std::string device_3_MAC;
  std::string device_4_MAC;
  std::string device_5_MAC;
  std::string device_6_MAC;
  std::string device_7_MAC;
  std::string device_8_MAC;

public:
  UserEntity(const std::string &username, const std::string &hashed_password,
             unsigned int salt);


  Subject get_subject() const;
  std::string getUsername() const;
  std::string getHashedPassword() const;
  unsigned int getSalt() const;
  int getpage_num() const;

  void set_device_1_MAC(const std::string &mac);
  std::string get_device_1_MAC() const;

  void set_device_2_MAC(const std::string &mac);
  std::string get_device_2_MAC() const;

  void set_device_3_MAC(const std::string &mac);
  std::string get_device_3_MAC() const;

  void set_device_4_MAC(const std::string &mac);
  std::string get_device_4_MAC() const;

  void set_device_5_MAC(const std::string &mac);
  std::string get_device_5_MAC() const;

  void set_device_6_MAC(const std::string &mac);
  std::string get_device_6_MAC() const;

  void set_device_7_MAC(const std::string &mac);
  std::string get_device_7_MAC() const;

  void set_device_8_MAC(const std::string &mac);
  std::string get_device_8_MAC() const;

  int check_validity_id(const std::string &mac) {
    if (get_device_1_MAC().compare(mac) == 0)
      return 1;
    else if (get_device_1_MAC().empty()) {
      set_device_1_MAC(mac);
      return -1;
    } else if (get_device_2_MAC().compare(mac) == 0) {
      return 2;
    } else if (get_device_2_MAC().empty()) {
      set_device_2_MAC(mac);
      return -2;
    } else if (get_device_3_MAC().compare(mac) == 0) {
      return 4;
    } else if (get_device_3_MAC().empty()) {
      set_device_3_MAC(mac);
      return -4;
    } else if (get_device_4_MAC().compare(mac) == 0) {
      return 8;
    } else if (get_device_4_MAC().empty()) {
      set_device_4_MAC(mac);
      return -8;
    } else if (get_device_5_MAC().compare(mac) == 0) {
      return 16;
    } else if (get_device_5_MAC().empty()) {
      set_device_5_MAC(mac);
      return -16;
    } else if (get_device_6_MAC().compare(mac) == 0) {
      return 32;
    } else if (get_device_6_MAC().empty()) {
      set_device_6_MAC(mac);
      return -32;
    } else if (get_device_7_MAC().compare(mac) == 0) {
      return 64;
    } else if (get_device_7_MAC().empty()) {
      set_device_7_MAC(mac);
      return -64;
    } else if (get_device_8_MAC().compare(mac) == 0) {
      return 128;
    } else if (get_device_8_MAC().empty()) {
      set_device_8_MAC(mac);
      return -128;
    }
    throw ExceededNumberOfDevices();
  }
};
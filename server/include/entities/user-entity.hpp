#pragma once
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
  size_t db_selected;

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

  void set_db_selected(const size_t db);
  size_t get_db_selected() const;

  int check_validity_id(const std::string &mac);
};
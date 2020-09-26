#pragma once
#include "json-serializable.hpp"
#include <string>

class SignupDTO : public JSONSerializable {
private:
  std::string username;
  std::string password;
  std::string password_confirm;
  std::string mac_address;

public:
  SignupDTO(){};

  std::string getUsername() const;
  std::string getPassword() const;
  std::string getPasswordConfirm() const;
  std::string getMAC() const;

  const virtual std::string serialize() {
    json_object = {{"username", username},
                   {"password", password},
                   {"password_confirm", password_confirm}};
    return json_object.dump();
  }
  const virtual void deserialize(const std::string &json_string) {
    json_object = json::parse(json_string);
    username = json_object["username"];
    password = json_object["password"];
    password_confirm = json_object["password_confirm"];
    mac_address = json_object["mac_address"];
  }
};

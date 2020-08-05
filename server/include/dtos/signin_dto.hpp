#pragma once
#include "json-serializable.hpp"

class SigninDTO : public JSONSerializable {
private:
  std::string username;
  std::string password;

public:
  SigninDTO(){};
  std::string getUsername() const;
  std::string getPassword() const;

  const virtual std::string serialize() {
    json_object = {{"username", username}, {"password", password}};
    return json_object.dump();
  }
  const virtual void deserialize(const std::string &json_string) {
    json_object = json::parse(json_string);
    username = json_object["username"];
    password = json_object["password"];
  }
};
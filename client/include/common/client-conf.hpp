#pragma once

#include "json.hpp"
#include <fstream>
#include <memory>
#include <string>

using json = nlohmann::json;

class ClientConf {
private:
  json content;
  static inline std::shared_ptr<ClientConf> instance{nullptr};
  ClientConf() {
    std::ifstream i("./config/client-conf.json");
    i >> content;
  }

public:
  static std::shared_ptr<ClientConf> getInstance() {
    if (instance.get() == nullptr) {
      instance = std::make_shared<ClientConf>(ClientConf());
    }
    return instance;
  }
  std::string getUser() const { return content["username"]; }
  std::string getTokenValue() const { return content["token"]; }
};
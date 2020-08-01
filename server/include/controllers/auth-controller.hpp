#pragma once
#include "controller.hpp"
#include "../dtos/user-log-dto.hpp"
#include "../services/user-service.hpp"
#include "../common/makereply.hpp"
#include <regex>

class AuthController : public Controller {

private:
  inline static AuthController *instance = nullptr;

public:
  // contiene switch_case per inoltrare al metodo corretto
  static AuthController *getInstance() {
    if (instance == nullptr) {
      instance = new AuthController();
    }
    return instance;
  }
  virtual const http::server::reply handle(const http::server::request &req);
  const http::server::reply post_sigin(const UserLogDTO &req);
  const http::server::reply post_signup(const UserLogDTO &req);
};

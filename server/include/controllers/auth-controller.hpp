#pragma once
#include "controller.hpp"

/**
 * Controller per richieste /auth:
 * /auth/signup
 * /auth/signin
 */
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
  const http::server::reply handle(const http::server::request &req);
  const http::server::reply post_sigin(const UserLogDTO &req);
  const http::server::reply post_signup(const http::server::request &req);
};

#pragma once
#include "controller.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../services/user-service.hpp"
#include <regex>
#include "../common/makereply.hpp"




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
  const std::string post_signin(const SigninDTO &req);
  const std::string post_signup(const SignupDTO &req);
};

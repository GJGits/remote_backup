#pragma once
#include "controller.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../services/user-service.hpp"
#include <regex>
#include "../common/singleton.hpp"
#include "../common/makereply.hpp"




class AuthController : public Controller, public Singleton<AuthController> {

private:
    friend class Singleton;
    std::shared_ptr<UserService> user_service;
    AuthController(){    this->user_service = UserService::getInstance();}
public:

  virtual const http::server::reply handle(const http::server::request &req);
  const std::string post_signin(const SigninDTO &req);
  const std::string post_signup(const SignupDTO &req);

};

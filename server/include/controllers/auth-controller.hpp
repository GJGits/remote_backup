#pragma once
#include "controller.hpp"

class AuthController : public Controller, public Singleton<AuthController> {

private:
  friend class Singleton;
  std::regex signin_rgx;
  std::regex signup_rgx;
  AuthController()
      : signin_rgx{std::move(std::regex{
            "^\\{\"username\":\\s?\"\\w+\",\\s?\"password\":\\s?\"\\w+\",\\s?"
            "\"mac_address\":\\s?\"[0-9a-f:]{17}\"\\}$"})},
        signup_rgx{std::move(
            std::regex{"^\\{\"username\":\\s?\"\\w+\",\\s?\"password\":\\s?"
                       "\"\\w+\",\\s?\"password_confirm\":\\s?\"\\w+\",\\s?"
                       "\"mac_address\":\\s?\"[0-9a-f:]{17}\"\\}$"})} {}

public:
  virtual const http::server::reply handle(const http::server::request &req);
  const std::string post_signin(const SigninDTO &req);
  const std::string post_signup(const SignupDTO &req);

  const std::regex get_signin_rgx();
  const std::regex get_signup_rgx();
};

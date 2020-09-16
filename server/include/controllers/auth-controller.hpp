#pragma once
#include "controller.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../services/user-service.hpp"
#include "../services/file-service.hpp"
#include <regex>
#include "../common/makereply.hpp"




class AuthController : public Controller {

private:
    static inline std::shared_ptr<AuthController> instance{nullptr};
    std::shared_ptr<UserService> user_service;
    std::shared_ptr<FileService> file_service;

public:
    static std::shared_ptr<AuthController> getInstance();

  virtual const http::server::reply handle(const http::server::request &req);
  const std::string post_signin(const SigninDTO &req);
  const std::string post_signup(const SignupDTO &req);

};

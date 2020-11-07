#include "../../include/controllers/auth-controller.hpp"


const std::regex AuthController::get_signin_rgx(){return signin_rgx;}
const std::regex AuthController::get_signup_rgx(){return signup_rgx;}

const http::server::reply
AuthController::handle(const http::server::request &req) {

  if (req.method == "POST") {

    if (req.uri == "/auth/signin") {
      std::string req_body = std::string{req.body.get()->begin(), req.body.get()->end()};
      std::smatch match;

      if (std::regex_match(req_body, match, signin_rgx)) {
          SigninDTO user{};
          user.deserialize(req_body);
          return MakeReply::make_1line_jsonReply<std::string>("token", post_signin(user), http::server::reply::ok);
      }
    } else if (req.uri == "/auth/signup") {
      std::string req_body{req.body.get()->begin(), req.body.get()->end()};
      std::smatch match;
      if (std::regex_match(req_body, match, signup_rgx)) {
          SignupDTO user{};
          user.deserialize(req_body);
          return MakeReply::make_1line_jsonReply<std::string>("token", post_signup(user), http::server::reply::ok);
      }
    }
  }
    throw WrongRquestFormat();
}

const std::string AuthController::post_signin(const SigninDTO &user) {
  std::shared_ptr<UserService> user_service = UserService::getInstance();
  return user_service->login(user);
}

const std::string AuthController::post_signup(const SignupDTO &user) {
    std::shared_ptr<UserService> user_service = UserService::getInstance();
    return user_service->signup(user);
}
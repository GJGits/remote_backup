#include "../../include/controllers/auth-controller.hpp"

inline static std::regex signin_rgx{"^\\{\"username\":\\s?\"\\w+\",\\s?\"password\":\\s?\"\\w+\",\\s?\"mac_address\":\\s?\"[0-9a-f:]{17}\"\\}$"};
inline static std::regex signup_rgx{"^\\{\"username\":\\s?\"\\w+\",\\s?\"password\":\\s?\"\\w+\",\\s?\"password_confirm\":\\s?\"\\w+\",\\s?\"mac_address\":\\s?\"[0-9a-f:]{17}\"\\}$"};

std::shared_ptr<AuthController> AuthController::getInstance() {
    if (instance.get() == nullptr) {
        instance = std::shared_ptr<AuthController>(new AuthController{});
        instance->user_service = UserService::getInstance();
    }
    return instance;
}

const http::server::reply
AuthController::handle(const http::server::request &req) {

  if (req.method == "POST") {

    if (req.uri == "/auth/signin") {
      std::string req_body = std::string{req.body.get()->begin(), req.body.get()->end()};
      std::clog << "signin body: " << req_body << "\n";
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
  return user_service->signup(user);
}
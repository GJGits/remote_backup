#include "../../include/controllers/auth-controller.hpp"

inline static std::regex signin_rgx{"^\\{\"username\":\\s?\"\\w+\",\\s?\"password\":\\s?\"\\w+\"\\}$"};
inline static std::regex signup_rgx{"^\\{\"username\":\\s?\"\\w+\",\\s?\"password\":\\s?\"\\w+\",\\s?\"password_confirm\":\\s?\"\\w+\"\\}$"};



const http::server::reply
AuthController::handle(const http::server::request &req) {

  if (req.method == "POST") {

    if (req.uri == "/auth/signin") {

      std::string req_body = std::string{req.body.begin(), req.body.end()};
      std::smatch match;
      if (!std::regex_match(req_body, match, signin_rgx))
        throw WrongRquestFormat(); // todo: creare eccezione

      SigninDTO user{};
      user.deserialize(req_body);
      return post_signin(user);

    } else if (req.uri == "/auth/signup") {

      std::string req_body = std::string{req.body.begin(), req.body.end()};
      std::smatch match;
      if (!std::regex_match(req_body, match, signup_rgx))
        throw WrongRquestFormat(); // todo: creare eccezione

      SignupDTO user{}; // specifica
      user.deserialize(req_body);
      return post_signup(user);
    }
  }
    throw WrongRquestFormat(); // todo: creare eccezione

}

const http::server::reply AuthController::post_signin(const SigninDTO &user) {

  UserService *user_service = UserService::getInstance();
  return MakeReply::make_1line_jsonAuthreply(user_service->login(user));
}

const http::server::reply AuthController::post_signup(const SignupDTO &user) {

  UserService *user_service = UserService::getInstance();
  return MakeReply::make_1line_jsonAuthreply(user_service->signup(user));
}
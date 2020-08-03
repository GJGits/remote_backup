#include "../../include/controllers/auth-controller.hpp"

inline static std::regex signin_rgx{
    "^{\"username\":\\s?\"\\w+\",\\s?\"password\":\\s?\"\\w+\"}$"};
inline static std::regex signup_rgx{
    "^{\"username\":\\s?\"\\w+\",\\s?\"password\":\\s?\"\\w+\",\\s?\"password_"
    "confirm\":\\s?\"\\w+\"}$"};
inline static std::regex user_rgx{"(\\w+)$"};

const http::server::reply
AuthController::handle(const http::server::request &req) {

  if (req.method == "POST") {

    if (req.uri == "/auth/signin") {

      std::string req_body = std::string{req.body.begin(), req.body.end()};
      std::smatch match;
      if (!std::regex_match(req_body, match, signin_rgx))
        throw CredentialsNotValidException(); // todo: creare eccezione

      SigninDTO user{};
      return post_sigin(user.deserialize(req_body));

    } else if (req.uri == "/auth/signup") {

      std::string req_body = std::string{req.body.begin(), req.body.end()};
      std::smatch match;
      if (!std::regex_match(req_body, match, signup_rgx))
        throw CredentialsNotValidException(); // todo: creare eccezione

      SignupDTO user{}; // specifica
      return post_signup(user.deserialize(req_body));
    }
  }
  return http::server::reply::stock_reply(
      http::server::reply::bad_request); // Sarà una rep vuota qui, ricordarsi
}

const http::server::reply AuthController::post_sigin(const UserLogDTO &user) {

  UserService *user_service = UserService::getInstance();
  std::string result = user_service->login(user);
  http::server::reply rep;
  rep.status = http::server::reply::ok;
  MakeReply::makereply(rep, user);
  return rep;
}

const http::server::reply AuthController::post_signup(const UserLogDTO &user) {

  UserService *user_service = UserService::getInstance();
  std::string result = user_service->signup(user);
  http::server::reply rep;
  rep.status = http::server::reply::ok;
  MakeReply::makereply(rep, user);
  return rep;
}
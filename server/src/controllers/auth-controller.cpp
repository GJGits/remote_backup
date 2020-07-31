#include "../../include/controllers/auth-controller.hpp"
#include "../../include/dtos/user-log-dto.hpp"
#include "../../include/error_message/error-message.hpp"
#include "../../include/services/user-service.hpp"
#include <regex>

inline static std::regex user_rgx{"(\\w+)$"};

const http::server::reply
AuthController::handle(const http::server::request &req) {

  if (req.method == "POST") {
    if (req.uri == "/auth/signin") {
      std::stringstream ss;
      for (auto c : req.body) {
        ss << c;
      }
      boost::property_tree::ptree pt;
      boost::property_tree::read_json(ss, pt);
      // todo: controllare che l'oggetto passato sia corretto
      std::string username(pt.get<std::string>("username", ""));
      std::string password(pt.get<std::string>("password", ""));
      std::smatch match;

      if (username.size() == 0 || password.size() == 0 ||
          !std::regex_match(username, match, user_rgx)) {
        return http::server::reply::stock_reply(
            http::server::reply::bad_request);
      }
      UserLogDTO user_dto{username, password};
      return post_sigin(user_dto);

    } else if (req.uri == "/auth/signup") {
      // std::clog << "sono in auth/signup della authcontroller\n";
      std::stringstream ss;
      for (auto c : req.body) {
        ss << c;
      }
      boost::property_tree::ptree pt;
      boost::property_tree::read_json(ss, pt);
      std::string username(pt.get<std::string>("username", ""));
      std::string password(pt.get<std::string>("password", ""));
      std::string password_confirm(pt.get<std::string>("password_confirm", ""));
      std::smatch match;

      if (username.size() == 0 || password.size() == 0 ||
          password_confirm.size() == 0 ||
          !std::regex_match(username, match, user_rgx)) {
        return http::server::reply::stock_reply(
            http::server::reply::bad_request);
      }

      UserLogDTO user_dto{username, password, password_confirm};
      return post_signup(user_dto);
    }
  }
  return http::server::reply::stock_reply(
      http::server::reply::bad_request); // Sarà una rep vuota qui, ricordarsi
}

const http::server::reply AuthController::post_sigin(const UserLogDTO &user) {

  // todo: sostituire con getInstance

  UserService *user_service = UserService::getInstance();
  std::tuple<error_enum, std::string> result = user_service->login(user);
  if (std::get<0>(result) == ok) {
    http::server::reply rep;
    rep.status = http::server::reply::ok;
    return rep;
  }
  http::server::reply rep;
  rep = http::server::reply::stock_reply(http::server::reply::bad_request);
  std::string reply_body = "{\"error\":\"" + std::get<1>(result) + "\"}";
  struct http::server::header con_len;
  con_len.name = "Content-Length";
  con_len.value = std::to_string(reply_body.size());
  struct http::server::header con_type;
  con_type.name = "Content-Type";
  con_type.value = "application/json";
  rep.headers.push_back(con_len);
  rep.headers.push_back(con_type);
  rep.content = reply_body;
  return rep; // Sarà una rep vuota qui, ricordarsi
}

const http::server::reply AuthController::post_signup(const UserLogDTO &user) {

  UserService *user_service = UserService::getInstance();
  std::tuple<error_enum, std::string> result = user_service->signup(user);

  if (std::get<0>(result) == ok) {
    http::server::reply rep;
    rep.status = http::server::reply::ok;
    return rep;
  }
  http::server::reply rep;
  rep = http::server::reply::stock_reply(http::server::reply::bad_request);
  std::string reply_body = "{\"error\":\"" + std::get<1>(result) + "\"}";
  struct http::server::header con_len;
  con_len.name = "Content-Length";
  con_len.value = std::to_string(reply_body.size());
  struct http::server::header con_type;
  con_type.name = "Content-Type";
  con_type.value = "application/json";
  rep.headers.push_back(con_len);
  rep.headers.push_back(con_type);
  rep.content = reply_body;
  return rep; // Sarà una rep vuota qui, ricordarsi
}
#include "../../include/controllers/status-controller.hpp"
#include "../../include/dtos/user-log-dto.hpp"
#include "../../include/services/user-service.hpp"
#include "../../include/http/header.hpp"
#include <regex>

// regex utente: "^\\w+$"
inline static std::regex user_rgx{"^/status/(\\w+)$"};

const http::server::reply
StatusController::handle(const http::server::request &req) {

  if (req.method == "GET") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match, user_rgx)) {
      std::clog << "match: " << match[1] << '\n';
      UserLogDTO user_dto{std::move(match[1])};
      return get_status(user_dto);
    }
  }
  return http::server::reply::stock_reply(http::server::reply::bad_request);
}

const http::server::reply StatusController::get_status(const UserLogDTO &user) {

  UserService *user_service = UserService::getInstance();
  std::tuple<error_enum ,std::string> result = user_service->getStatus(user);
  if (std::get<0>(result)==ok) {
    http::server::reply rep;
    rep.status = http::server::reply::ok;
    std::string reply_body = "{\"hashed_status\":\""+std::get<1>(result)+"\"}";
    struct http::server::header con_len;
    con_len.name = "Content-Length";
    con_len.value = std::to_string(reply_body.size());
    struct http::server::header con_type;
    con_type.name = "Content-Type";
    con_type.value = "application/json";
    rep.headers.push_back(con_len);
    rep.headers.push_back(con_type);
    rep.content = reply_body;
    return rep;
  } else {
      http::server::reply rep;
      rep = http::server::reply::stock_reply(http::server::reply::bad_request);
      std::string reply_body = "{\"error\":\""+std::get<1>(result)+"\"}";
      struct http::server::header con_len;
      con_len.name = "Content-Length";
      con_len.value = std::to_string(reply_body.size());
      struct http::server::header con_type;
      con_type.name = "Content-Type";
      con_type.value = "application/json";
      rep.headers.push_back(con_len);
      rep.headers.push_back(con_type);
      rep.content = reply_body;
      return rep;

  }

  return http::server::reply::stock_reply(http::server::reply::bad_request);
}
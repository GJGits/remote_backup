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
      std::clog << "sono in status del statuscontrollerhandle\n";
      return get_status(user_dto);
    }
  }
  return http::server::reply::stock_reply(http::server::reply::bad_request);
}

const http::server::reply StatusController::get_status(const UserLogDTO &user) {

  UserService *user_service = UserService::getInstance();
  std::optional<std::string> result = user_service->getStatus(user);
  if (result.has_value()) {
    http::server::reply rep;
    rep.status = http::server::reply::ok;
    std::string reply_body = "{\"hashed_status\":\"" + result.value() + "\"}";
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
    return http::server::reply::stock_reply(http::server::reply::bad_request);
  }

  return http::server::reply::stock_reply(http::server::reply::bad_request);
}
#include "../../include/controllers/status-controller.hpp"

// regex utente: "^\\w+$"
inline static std::regex user_rgx{"^/status/(\\w+)$"};

const http::server::reply
StatusController::handle(const http::server::request &req) {

  if (req.method == "GET") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match, user_rgx)) {
      UserLogDTO user_dto{std::move(match[1])};
      return get_status(user_dto);
    }
  }
  return http::server::reply::stock_reply(http::server::reply::bad_request);
}

const http::server::reply StatusController::get_status(const UserLogDTO &user) {

  UserService *user_service = UserService::getInstance();
  std::string result = user_service->getStatus(user);
    http::server::reply rep;
    rep.status = http::server::reply::ok;
    std::string reply_body = "{\"hashed_status\":\"" + result + "\"}";
    MakeReply::makereply(rep,reply_body);
    return rep;



}
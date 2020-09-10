#include "../../include/controllers/status-controller.hpp"

// regex utente: "^\\w+$"
inline static std::regex user_rgx{"^/status/(\\w+)$"};
inline static std::regex user_rgx_file{"^/status/(\\w+)/file$"};

const http::server::reply StatusController::handle(const http::server::request &req) {

  if (req.method == "GET") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match, user_rgx)) {
      //if (JWT::validateToken(req)) {
        std::string username{std::move(match[1])};
        return MakeReply::make_1line_jsonReply<std::string>(std::string{"hashed_status"}, get_status(username),http::server::reply::ok);
      //}
    } else if (std::regex_search(req.uri.begin(), req.uri.end(), match, user_rgx_file)) {
        //if (JWT::validateToken(req)) { sto codice è per l'autenticazione, rimetterlo poi
        std::string username{std::move(match[1])};
        return MakeReply::make_1line_dump_jsonReply<std::string>(get_status_file(username),http::server::reply::ok);
       // }
    }
  }
  throw WrongRquestFormat();
}

const std::string StatusController::get_status(const std::string &username) {

  UserService *user_service = UserService::getInstance();
  return user_service->getStatus(username);
}

const std::string StatusController::get_status_file(const std::string &username) {

    UserService *user_service = UserService::getInstance();
    return user_service->getStatusFile(username);
}
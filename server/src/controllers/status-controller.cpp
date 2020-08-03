#include "../../include/controllers/status-controller.hpp"

// regex utente: "^\\w+$"
inline static std::regex user_rgx{"^/status/(\\w+)$"};

const http::server::reply
StatusController::handle(const http::server::request &req) {

  if (req.method == "GET") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match, user_rgx)) {
      std::string username{std::move(match[1])};
      return get_status(username);
    }
    else
        throw WrongRquestFormat(); // todo: creare eccezione
  }
    throw WrongRquestFormat(); // todo: creare eccezione
}

const http::server::reply StatusController::get_status(const std::string &username) {

  UserService *user_service = UserService::getInstance();
  std::string result = user_service->getStatus(username);
    http::server::reply rep;
    rep.status = http::server::reply::ok;
    json reply_body;
    reply_body["hashed_status"] = result;
    MakeReply::makereply(rep,reply_body);
    return rep;



}
#include "../../include/controllers/status-controller.hpp"

// regex utente: "^\\w+$"
inline static std::regex user_rgx{"^/status/(\\w+)$"};

http::server::reply make_1line_jsonreply(std::string result){
    http::server::reply rep;
    rep.status = http::server::reply::ok;
    json reply_body;
    reply_body["token"] = result;
    MakeReply::makejsonreply(rep, reply_body);
    return rep;
}

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
  return MakeReply::make_1line_jsonAuthreply(user_service->getStatus(username));



}
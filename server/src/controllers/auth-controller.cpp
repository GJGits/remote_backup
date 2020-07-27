#include "../../include/controllers/auth-controller.hpp"

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
      UserLogDTO user_dto{pt.get<std::string>("username"),
                          pt.get<std::string>("password")};
      // std::clog << "sono in auth/signin della authcontroller\n";
      return post_sigin(user_dto);
    } else if (req.uri == "/auth/signup") {
      // std::clog << "sono in auth/signup della authcontroller\n";
      return post_signup(req);
    }
  }
  return http::server::reply::stock_reply(
      http::server::reply::bad_request); // Sarà una rep vuota qui, ricordarsi
}

const http::server::reply AuthController::post_sigin(const UserLogDTO &user) {

  // todo: sostituire con getInstance
  UserService service{};
  bool result = service.login(user);
  if (result) {
    http::server::reply rep;
    rep.status = http::server::reply::ok;
    std::clog << "sono nella authcontroller::postsignin\n";
    return rep;
  }
  http::server::reply rep;
  rep = http::server::reply::stock_reply(http::server::reply::bad_request);
  rep.content = "{'error':'credenziali non valide'}";
  return rep; // Sarà una rep vuota qui, ricordarsi
}

const http::server::reply
AuthController::post_signup(const http::server::request &req) {

  http::server::reply rep;
  rep.status = http::server::reply::ok;
  std::clog << "sono nella authcontroller::postsignup\n";
  return rep;
}
#include "../../include/controllers/auth-controller.hpp"
#include "../../include/dtos/user-log-dto.hpp"
#include "../../include/services/user-service.hpp"

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
      std::string username(pt.get<std::string>("username",""));
      std::string password(pt.get<std::string>("password",""));
      if(username.size() == 0 || password.size() == 0){
          return http::server::reply::stock_reply(
                  http::server::reply::bad_request);
      }
      UserLogDTO user_dto{username,password};
      return post_sigin(user_dto);

    } else if (req.uri == "/auth/signup") {
      // std::clog << "sono in auth/signup della authcontroller\n";
        std::stringstream ss;
        for (auto c : req.body) {
            ss << c;
        }
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);
        std::string username(pt.get<std::string>("username",""));
        std::string password(pt.get<std::string>("password",""));
        std::string password_confirm(pt.get<std::string>("password_confirm",""));
        if(username.size() == 0 || password.size() == 0 || password_confirm.size() == 0){
            return http::server::reply::stock_reply(http::server::reply::bad_request);
        }
        /* Eventualmente tale check se deve essere fatto più avanti lo si sposta*/
        if(password.compare(password_confirm)!=0)
            return http::server::reply::stock_reply(http::server::reply::bad_request);

        UserLogDTO user_dto{username,password,password_confirm};
      return post_signup(user_dto);
    }
  }
  return http::server::reply::stock_reply(
      http::server::reply::bad_request); // Sarà una rep vuota qui, ricordarsi
}

const http::server::reply AuthController::post_sigin(const UserLogDTO &user) {

  // todo: sostituire con getInstance
  UserService *user_service = UserService::getInstance();
  bool result = user_service->login(user);
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
AuthController::post_signup(const UserLogDTO &user) {

  http::server::reply rep;
  rep.status = http::server::reply::ok;
  std::clog << "sono nella authcontroller::postsignup\n";
  return rep;
}
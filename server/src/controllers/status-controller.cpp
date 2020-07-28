#include "../../include/controllers/status-controller.hpp"
#include "../../include/services/user-service.hpp"
#include "../../include/dtos/user-log-dto.hpp"

const http::server::reply
StatusController::handle(const http::server::request &req) {
  if (req.method == "GET") {
    if (req.uri == "/status") {

        std::stringstream ss;
        for (auto c : req.body) {
            ss << c;
        }
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);
        // todo: controllare che l'oggetto passato sia corretto
        std::string username(pt.get<std::string>("username",""));
        if(username.size() == 0)
            return http::server::reply::stock_reply(http::server::reply::bad_request);
        UserLogDTO user_dto{username};

        std::clog << "sono in status del statuscontrollerhandle\n";
      return get_status(user_dto);
    }
  }
  return http::server::reply::stock_reply(http::server::reply::bad_request);
}

const http::server::reply
StatusController::get_status(const UserLogDTO &user) {

    UserService *user_service = UserService::getInstance();
    std::string result = user_service->getStatus(user);
    std::clog << "Il ritorno è: " << result << "\n";
    if(result.size()>0) {
        http::server::reply rep;
        rep.status = http::server::reply::ok;
        rep.content = result;
        return rep;
    }
    else{
        return http::server::reply::stock_reply(http::server::reply::bad_request);
    }

  return http::server::reply::stock_reply(http::server::reply::bad_request);
}
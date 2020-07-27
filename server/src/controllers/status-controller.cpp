#include "../../include/controllers/status-controller.hpp"

const http::server::reply
StatusController::handle(const http::server::request &req) {
  if (req.method == "GET") {
    if (req.uri == "/status") {
      std::clog << "sono in status del statuscontrollerhandle\n";
      return get_status(req);
    }
  }
  return http::server::reply::stock_reply(http::server::reply::bad_request);
}

const http::server::reply
StatusController::get_status(const http::server::request &req) {

  http::server::reply rep;
  rep.status = http::server::reply::ok;
  std::clog << "sono nella StatusController::get_status\n";
  return rep;
}
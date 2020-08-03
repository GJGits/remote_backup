//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "../../include/http/request_handler.hpp"


namespace http {
namespace server {

request_handler::request_handler(const std::string &doc_root)
    : doc_root_(doc_root) {}

void request_handler::handle_request(const request &req, reply &rep) {
  // Decode url to path.
  std::string request_path;

  try {

    std::optional<Controller *> c = ControllerRouter::getController(req.uri);
    if (c.has_value()) {
      // qui ok
      rep = c.value()->handle(req);
    } else {
      // qui arriva solo se non esiste un controller
      rep = reply::stock_reply(reply::bad_request);
      return;
    }
  } catch (UsernameAlreadyExists &e) {
    std::string what{e.what()};
      rep = http::server::reply::stock_reply(http::server::reply::internal_server_error);
     json reply_body =  {"error", what};
    MakeReply::makereply(rep,reply_body);
    return;
  } catch (CredentialsNotValidException &e) {
      std::string what{e.what()};
      rep = http::server::reply::stock_reply(http::server::reply::internal_server_error);
     json reply_body =  {"error", what};
      MakeReply::makereply(rep,reply_body);
      return;
  }catch (UsernameNotExists &e) {
      std::string what{e.what()};
      rep = http::server::reply::stock_reply(http::server::reply::internal_server_error);
      json reply_body =  {"error", what};
      return MakeReply::makereply(rep,reply_body);
  }catch (PasswordNeqConfirm &e) {
      std::string what{e.what()};
      rep = http::server::reply::stock_reply(http::server::reply::internal_server_error);
      json reply_body =  {"error", what};
      MakeReply::makereply(rep,reply_body);
      return;
  }catch (UknownError &e) {
      std::string what{e.what()};
      rep = http::server::reply::stock_reply(http::server::reply::internal_server_error);
      json reply_body =  {"error", what};
      MakeReply::makereply(rep,reply_body);
      return;
  }

}

} // namespace server
} // namespace http

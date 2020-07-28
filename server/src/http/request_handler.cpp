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
#include "../../include/http/mime_types.hpp"
#include "../../include/http/reply.hpp"
#include "../../include/http/request.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace http {
namespace server {

request_handler::request_handler(const std::string &doc_root)
    : doc_root_(doc_root) {}

void request_handler::handle_request(const request &req, reply &rep) {
  // Decode url to path.
  std::string request_path;

  std::optional<Controller *> c = ControllerRouter::getController(req.uri);
  if (c.has_value()) {
    // qui ok
    rep = c.value()->handle(req);
  } else {
    // qui arriva solo se non esiste un controller
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  // Fill out the reply to be sent to the client.
  /*
  rep.status = reply::ok;
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(4);
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = std::string{"text/plain"};
  rep.content = std::string{"ciao"};
  std::clog << "QUI DAL SERVER... TUTTO OK\n";
  */
}

} // namespace server
} // namespace http

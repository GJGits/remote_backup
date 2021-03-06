//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once


#include "../common/makereply.hpp"
#include "../controllers/auth-controller.hpp"
#include "../controllers/status-controller.hpp"
#include "../controllers/controller-router.hpp"
#include <string>
#include "../exceptions/exceptions.hpp"
#include "../http/mime_types.hpp"
#include "../http/reply.hpp"
#include "../http/request.hpp"
#include "../common/json.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

namespace http {
namespace server {

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler {
public:
  request_handler(const request_handler &) = delete;
  request_handler &operator=(const request_handler &) = delete;

  /// Construct with a directory containing files to be served.
  explicit request_handler(const std::string &doc_root);

  /// Handle a request and produce a reply.
  void handle_request(const request &req, reply &rep);

private:
  /// The directory containing the files to be served.
  std::string doc_root_;

};

} // namespace server
} // namespace http

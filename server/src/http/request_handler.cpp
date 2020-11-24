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
  try {
    std::shared_ptr<ControllerRouter> router = ControllerRouter::getInstance();
    std::shared_ptr<Controller> c = router->getController(req.uri);
    rep = c->handle(req);
    return;

  } catch (UsernameAlreadyExists &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::bad_request);
    return;

  } catch (CredentialsNotValidException &e) {
    std::clog << e.what() << "\n";
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::bad_request);
    return;

  } 
  catch (exceededSize &e) {
    std::clog << e.what() << "\n";
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::size_exceeded);
    return;

  }
  catch (WrongRquestFormat &e) {
    std::clog << e.what() << "\n";
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::bad_request);
    return;

  } catch (CredentialsExpired &e) {
    std::clog << e.what() << "\n";
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (ChunkCorrupted &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::bad_request);
    return;
  } catch (sql::SQLException &e) {
    std::string error_message;
    error_message = std::string{"Error code:  "} + e.what() +
                    std::string{"___ Error Explanation: "} +
                    std::to_string(e.getErrorCode()) +
                    std::string{"___ SQL STATE: "} + e.getSQLState();

    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", error_message, http::server::reply::internal_server_error);
    return;
  } catch (FileNotOpened &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (std::filesystem::filesystem_error &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (nlohmann::json::exception &e) {
    std::clog << e.what() << "\n";
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (InvalidJWT &e) {
    std::clog << e.what() << "\n";
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::unauthorized);
    return;
  } catch (Base64DecodeFailed &e) {
    std::clog << e.what() << "\n";
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (ExceededNumberOfDevices &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (MissingElementInDB &e) {
    std::clog << e.what() << "\n";
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (std::invalid_argument &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::bad_request);
    return;
  } catch (std::out_of_range &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::bad_request);
    return;
  }
}

} // namespace server
} // namespace http

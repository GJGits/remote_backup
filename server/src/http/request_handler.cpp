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

    std::shared_ptr<Controller> c = ControllerRouter::getController(req.uri);
    rep = c->handle(req);
    return;

  } catch (UsernameAlreadyExists &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;

  } catch (CredentialsNotValidException &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;

  } catch (UsernameNotExists &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;

  } catch (PasswordNeqConfirm &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;

  } catch (WrongRquestFormat &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;

  } catch (ControllerNotRetrievable &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;

  } catch (UknownError &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;

  } catch (CredentialsExpired &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (ChunkCorrupted &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (FileSizeNotAvailable &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (sql::SQLException &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error",
        std::string{"Error code:  "} + e.what() +
            std::string{"___ Error Explanation: "} +
            std::to_string(e.getErrorCode()) + std::string{"___ SQL STATE: "} +
            e.getSQLState(),
        http::server::reply::internal_server_error);
    return;
  } catch (FileNotDeleted &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (FileNotOpened &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  } catch (std::filesystem::__cxx11::filesystem_error &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  }

  catch (DatabaseInvalidConnection &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  }

  catch (nlohmann::json::exception &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  }

  catch (InvalidJWT &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  }

  catch (Base64DecodeFailed &e) {
    rep = MakeReply::make_1line_jsonReply<std::string>(
        "error", e.what(), http::server::reply::internal_server_error);
    return;
  }

   catch(ExceededNumberOfDevices &e){
       rep = MakeReply::make_1line_jsonReply<std::string>(
               "error", e.what(), http::server::reply::internal_server_error);
       return;
    }
  catch(MissingElementInDB &e){
      rep = MakeReply::make_1line_jsonReply<std::string>(
              "error", e.what(), http::server::reply::internal_server_error);
      return;
  }

}


} // namespace server
} // namespace http

/**
 * QUESTE CLASSI DEVONO ESSERE SIGNLETON
 */

#pragma once

#include "reply.hpp"
#include "request.hpp"
#include <optional>
#include <string>
#include <unordered_map>

class Controller {};

/**
 * Controller per richieste /auth:
 * /auth/signup
 * /auth/signin
 */
class AuthController : public Controller {

private:
  static AuthController *instance;

public:
  // contiene switch_case per inoltrare al metodo corretto
  static AuthController *getInstance() {
    if (instance == nullptr) {
      instance = new AuthController();
    }
    return instance;
  }
  http::server::reply handle(const http::server::request &req);
  http::server::reply post_sigin(const http::server::request &req);
  http::server::reply post_signup(const http::server::request &req);
};

/**
 * Controller per richieste /status
 */
class StatusController : public Controller {
private:
  static StatusController *instance;

public:
  // contiene switch_case per inoltrare al metodo corretto
  static StatusController *getInstance() {
    if (instance == nullptr) {
      instance = new StatusController();
    }
    return instance;
  }
  http::server::reply handle(const http::server::request &req);
  http::server::reply get_status(const http::server::request &req);
};

class ControllerRouter {
private:
  static std::unordered_map<std::string, Controller *> controller_map;
  static ControllerRouter *instance;

public:
  static std::optional<Controller *> getController(const std::string &key) {
    // se mappa non inizializzata
    if (instance == nullptr) {
      instance = new ControllerRouter();
      ControllerRouter::controller_map["auth"] = AuthController::getInstance();
      // todo: inizializza
    }
    // qui map ok, recupero controller
    // todo: recupera controller qui:
    // se trovo return std::optional<Controller *>
    // ControllerRouter::instance.controller_map[key]; else std::nullopt;
    return std::optional<Controller *>(AuthController::getInstance());
  }
};

ControllerRouter *ControllerRouter::instance = nullptr;
AuthController *AuthController::instance = nullptr;
StatusController *StatusController::instance = nullptr;
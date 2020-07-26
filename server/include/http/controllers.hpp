/**
 * QUESTE CLASSI DEVONO ESSERE SIGNLETON
 */

#pragma once

#include "reply.hpp"
#include "request.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <iostream>

class Controller {

public:
	virtual std::string handle(const http::server::request &req) = 0;

};

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
  std::string handle(const http::server::request &req);
  std::string post_sigin(const http::server::request &req);
  std::string post_signup(const http::server::request &req);
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
  std::string handle(const http::server::request &req);
  std::string get_status(const http::server::request &req);
};

static std::unordered_map<std::string, Controller *> controller_map;

class ControllerRouter {
private:

  static ControllerRouter *instance;

public:
  static std::optional<Controller *> getController(const std::string &key) {
    // se mappa non inizializzata
    if (instance == nullptr) {
      instance = new ControllerRouter();
      controller_map["auth"] = AuthController::getInstance();
      controller_map["status"] = StatusController::getInstance();
      // todo: inizializza
    }
    // qui map ok, recupero controller
    // todo: recupera controller qui:
    std::string analysis_stringa(key);
    char *res = std::strtok (const_cast<char *>(analysis_stringa.c_str()), "/");
    std::clog << key << "\n";
    if(controller_map[res] != nullptr){
    	if(strcmp(res , "auth") == 0)
    		return std::optional<Controller *>(AuthController::getInstance());
    	else if (strcmp(res , "status") == 0) 
    	    	return std::optional<Controller *>(StatusController::getInstance());
    
    }
    
    
    return std::optional<Controller *>();
  }
};


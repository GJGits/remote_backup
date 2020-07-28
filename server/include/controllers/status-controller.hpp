#pragma once
#include "controller.hpp"

/**
 * Controller per richieste /status
 */
class StatusController : public Controller {
private:
  inline static StatusController *instance = nullptr;

public:
  // contiene switch_case per inoltrare al metodo corretto
  static StatusController *getInstance() {
    if (instance == nullptr) {
      instance = new StatusController();
    }
    return instance;
  }
  virtual const http::server::reply handle(const http::server::request &req);
  const http::server::reply get_status(const http::server::request &req);
};
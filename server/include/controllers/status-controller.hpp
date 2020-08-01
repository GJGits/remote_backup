#pragma once
#include "controller.hpp"
#include "../dtos/user-log-dto.hpp"
#include "../services/user-service.hpp"
#include "../http/header.hpp"
#include "../common/makereply.hpp"
#include <regex>

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
  const http::server::reply get_status(const UserLogDTO &req);
};
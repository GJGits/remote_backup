#pragma once
#include "controller.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../services/user-service.hpp"
#include "../http/header.hpp"
#include <regex>
#include "../common/makereply.hpp"




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
  const std::string get_status(const std::string &username);

};
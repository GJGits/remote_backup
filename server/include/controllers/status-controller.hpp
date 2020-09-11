#pragma once
#include "controller.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../services/user-service.hpp"
#include "../services/file-service.hpp"
#include "../http/header.hpp"
#include <regex>
#include "../common/makereply.hpp"




class StatusController : public Controller {
private:
    static inline std::shared_ptr<StatusController> instance{nullptr};

public:
    static std::shared_ptr<StatusController> getInstance();

    virtual const http::server::reply handle(const http::server::request &req);
  const std::string get_status(const std::string &username);
  const std::string get_status_file(const std::string &username);
};
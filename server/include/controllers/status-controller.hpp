#pragma once
#include "../dtos/get_status_dto.hpp"
#include "../services/file-service.hpp"
#include "../common/jwt.hpp"
#include "../common/makereply.hpp"
#include "../common/singleton.hpp"
#include "controller.hpp"
#include <regex>

class StatusController : public Controller, public Singleton<StatusController> {
private:
    friend class Singleton;
    std::regex user_rgx_file;
    StatusController(): user_rgx_file{std::move(std::regex{"^\\/status\\/list\\/[\\d]+\\/[\\d]+$"})}{}

public:

    virtual const http::server::reply handle(const http::server::request &req);
  const json get_status_file(const GetStatusDTO &get_status_dto);
  const std::regex get_user_rgx_file();
};
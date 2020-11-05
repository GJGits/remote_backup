#pragma once
#include "controller.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/get_status_dto.hpp"
#include "../services/file-service.hpp"
#include "../dtos/subject.hpp"
#include "../http/header.hpp"
#include <regex>
#include "../common/makereply.hpp"
#include "../common/singleton.hpp"




class StatusController : public Controller, public Singleton<StatusController> {
private:
    friend class Singleton;
    StatusController(){}

public:

    virtual const http::server::reply handle(const http::server::request &req);
  const json get_status_file(const GetStatusDTO &get_status_dto);
};
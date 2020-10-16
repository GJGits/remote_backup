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




class StatusController : public Controller {
private:
    static inline std::shared_ptr<StatusController> instance{nullptr};
    std::shared_ptr<FileService> file_service;

public:
    static std::shared_ptr<StatusController> getInstance();

    virtual const http::server::reply handle(const http::server::request &req);
  const json get_status_file(const GetStatusDTO &get_status_dto);
};
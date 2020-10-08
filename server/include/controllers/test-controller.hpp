#pragma once
#include "controller.hpp"
#include "../dtos/get_test_database_dto.hpp"
#include "../services/test-service.hpp"

#include "../dtos/subject.hpp"
#include "../http/header.hpp"
#include <regex>
#include "../common/makereply.hpp"


class TestController : public Controller {
private:
    static inline std::shared_ptr<TestController> instance{nullptr};
    std::shared_ptr<TestService> test_service;

public:
    static std::shared_ptr<TestController> getInstance();

    virtual const http::server::reply handle(const http::server::request &req);
    const json get_test_database_table_name(const GetTestDatabaseDTO &get_test_database);
};
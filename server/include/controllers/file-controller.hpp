#pragma once
#include "controller.hpp"
#include "../common/makereply.hpp"
#include "../common/base64.hpp"
#include "../common/sha256.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/post_file_dto.hpp"
#include "../services/user-service.hpp"
#include <regex>
#include "../common/utility.hpp"

class FileController : public Controller {

private:
    inline static FileController *instance = nullptr;

public:
    // contiene switch_case per inoltrare al metodo corretto
    static FileController *getInstance() {
        if (instance == nullptr) {
            instance = new FileController();
        }
        return instance;
    }
    virtual const http::server::reply handle(const http::server::request &req);

};
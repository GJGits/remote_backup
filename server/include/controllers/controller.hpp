#pragma once

#include "../http/reply.hpp"
#include "../http/request.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/subject.hpp"
#include "../services/chunk-service.hpp"
#include "../services/user-service.hpp"
#include "../services/file-service.hpp"
#include "../common/jwt.hpp"
#include "../common/makereply.hpp"
#include "../common/singleton.hpp"
#include "../common/sha256.hpp"
#include <regex>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>


class Controller{
public:
    virtual const http::server::reply handle(const http::server::request &req) = 0;
};
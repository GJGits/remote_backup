#pragma once

#include "../http/reply.hpp"
#include "../http/request.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>



class Controller {

public:
	virtual const http::server::reply handle(const http::server::request &req) = 0;

};
#pragma once

#include "../http/reply.hpp"
#include "../http/request.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <iostream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <sstream>

class Controller {

public:
	virtual const http::server::reply handle(const http::server::request &req) = 0;
};
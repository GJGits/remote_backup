#pragma once

#include "../http/reply.hpp"
#include "../http/request.hpp"
#include "../dtos/user-log-dto.hpp"
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
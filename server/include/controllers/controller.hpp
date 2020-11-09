#pragma once

#include "../http/reply.hpp"
#include "../http/request.hpp"



class Controller{
public:
    virtual const http::server::reply handle(const http::server::request &req) = 0;
};
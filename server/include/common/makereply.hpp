#pragma once
#include "../../include/http/reply.hpp"
#include "../../include/http/request.hpp"

class MakeReply
{
public:
    static void makereply(http::server::reply &rep, std::string reply_body) {
        struct http::server::header con_len;
        con_len.name = "Content-Length";
        con_len.value = std::to_string(reply_body.size());
        struct http::server::header con_type;
        con_type.name = "Content-Type";
        con_type.value = "application/json";
        rep.headers.push_back(con_len);
        rep.headers.push_back(con_type);
        rep.content = reply_body;
        return;
    } // static member function
};
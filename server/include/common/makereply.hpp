#pragma once
#include "../../include/http/reply.hpp"
#include "../../include/http/request.hpp"

class MakeReply
{
public:


    static http::server::reply make_1line_jsonAuthreply(std::string result){
        http::server::reply rep;
        rep.status = http::server::reply::ok;
        json reply_body;
        reply_body["token"] = result;
        MakeReply::makejsonreply(rep, reply_body);
        return rep;
    }

    static http::server::reply make_1line_jsonStatusreply(std::string result){
        http::server::reply rep;
        rep.status = http::server::reply::ok;
        json reply_body;
        reply_body["hashed_status"] = result;
        MakeReply::makejsonreply(rep, reply_body);
        return rep;
    }

    static void makejsonreply(http::server::reply &rep, const json &rep_body) {
        struct http::server::header con_len;
        std::string rep_content = rep_body.dump();
        con_len.name = "Content-Length";
        con_len.value = std::to_string(rep_content.size());
        struct http::server::header con_type;
        con_type.name = "Content-Type";
        con_type.value = "application/json";
        rep.headers.push_back(con_len);
        rep.headers.push_back(con_type);
        rep.content = rep_content;
        return;
    } // static member function
};
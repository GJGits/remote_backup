#pragma once
#include "../../include/http/reply.hpp"
#include "../../include/http/request.hpp"
#include "json.hpp"
#include <string.h>
#include <vector>
#include <iostream>

using json = nlohmann::json;

class MakeReply {
public:
  template <typename V>
  static http::server::reply
  make_1line_jsonReply(std::string key, V value,
                       http::server::reply::status_type status) {
    http::server::reply rep = http::server::reply::stock_reply(status);
    json reply_body;
    reply_body[key] = value;
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
        for (size_t i = 0; i < rep_content.size(); i++) {
          (*rep.content)[i] = rep_content[i];
        }
    return;
  } // static member function

  template <typename V>
  static http::server::reply
  make_1line_dump_jsonReply(V value, http::server::reply::status_type status) {
    http::server::reply rep = http::server::reply::stock_reply(status);
    MakeReply::makejsonreply(rep, value);
    return rep;
  }

  static void makedumpjsonreply(http::server::reply &rep,
                                const std::string &rep_body) {
    struct http::server::header con_len;
    con_len.name = "Content-Length";
    con_len.value = std::to_string(rep_body.size());
    struct http::server::header con_type;
    con_type.name = "Content-Type";
    con_type.value = "application/json";
    rep.headers.push_back(con_len);
    rep.headers.push_back(con_type);
    std::shared_ptr<std::array<char, CHUNK_SIZE>> buff{
        new std::array<char, CHUNK_SIZE>{}};
        for (size_t i = 0; i < rep_body.size(); i++) {
          buff->data()[i] = rep_body[i];
        }
    return;
  } // static member function

};
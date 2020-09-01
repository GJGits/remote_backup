#pragma once

#include "../common/json.hpp"

using json = nlohmann::json;

class Message {
private:
  json content;

public:
  Message() {}
  Message(const json &content) : content{content} {}
  bool has_content() const {return this->content.is_null();}
  json get_content() const { return this->content; }
};
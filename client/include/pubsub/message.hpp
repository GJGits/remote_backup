#pragma once

#include "topics.hpp"
#include "../common/json.hpp"

using json = nlohmann::json;

class Message {
private:
  TOPIC topic;
  json content;

public:
  Message () {}
  Message(TOPIC topic) : topic{topic} {}
  Message(TOPIC topic, const json &content) : topic{topic} {
    this->content = content;
  }
  TOPIC get_topic() const { return this->topic; }
  bool has_content() const { return this->content.is_null(); }
  json get_content() const { return this->content; }
};
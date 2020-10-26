#pragma once

#include "../common/json.hpp"
#include "topics.hpp"
#include <memory>

using json = nlohmann::json;

class Message {
private:
  TOPIC topic;
  json content;

public:
  Message() {}
  Message(TOPIC topic) : topic{topic} {}
  Message(TOPIC topic, json &content) : topic{topic} {
    this->content = content;
  }
  TOPIC get_topic() const { return topic; }
  json get_content() const { return content; }
};

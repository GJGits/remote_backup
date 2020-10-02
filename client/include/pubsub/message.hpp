#pragma once

#include "../common/json.hpp"
#include "topics.hpp"
#include <memory>

using json = nlohmann::json;

// interfaccia

class Message {
private:
  TOPIC topic;
  json content1;
  std::shared_ptr<std::vector<char>> content2;

public:
  Message() {}
  Message(TOPIC topic) : topic{topic} {}
  Message(TOPIC topic, json &content) : topic{topic} {
    this->content1 = content;
  }
  Message(TOPIC topic, std::shared_ptr<std::vector<char>> &content)
      : topic{topic} {
    this->content2 = content;
  }
  TOPIC get_topic() const { return topic; }
  json get_content() const { return content1; }
  std::shared_ptr<std::vector<char>> get_content2() const { return content2; }
  void set_content(const json &content) {this->content1 = content;}
};

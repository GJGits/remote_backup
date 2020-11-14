#pragma once

#include "../filesystem/file_entry.hpp"
#include "topics.hpp"
#include <memory>

class Message {
private:
  TOPIC topic;
  std::shared_ptr<FileEntry> content;

public:
  Message() {}
  Message(TOPIC topic) : topic{topic} {}
  Message(TOPIC topic, const std::shared_ptr<FileEntry> &content) : topic{topic}, content{content} {}
  TOPIC get_topic() const { return topic; }
  std::shared_ptr<FileEntry> get_content() const { return content; }
};

#pragma once

#include <iostream>
#include <memory>

#include "broker.hpp"

class StructSubscriber {
private:
  static inline std::shared_ptr<StructSubscriber> instance{nullptr};
  StructSubscriber() {}

public:
  static std::shared_ptr<StructSubscriber> getInstance();
  void init();
  void on_add_chunk(const Message &message);
  void on_update_chunk(const Message &message);
  void on_delete_chunk(const Message &message);
  void on_delete_entry(const Message &message);
  void on_timeout(const Message &message);
};
#pragma once

#include <iostream>
#include <memory>
#include <mutex>

#include "../filesystem/sync_structure.hpp"
#include "broker.hpp"

class StructSubscriber {
private:
  std::mutex m;
  static inline std::shared_ptr<StructSubscriber> instance{nullptr};
  StructSubscriber() {}

public:
  static std::shared_ptr<StructSubscriber> getInstance();
  void init();
  void on_add_chunk(const Message &message);
  void on_file_modified(const Message &message);
  void on_delete_entry(const Message &message);
};
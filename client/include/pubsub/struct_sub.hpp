#pragma once

#include <iostream>
#include <memory>
#include <mutex>

#include "../common/singleton.hpp"
#include "../filesystem/sync_structure.hpp"
#include "broker.hpp"

class StructSubscriber : public Singleton<StructSubscriber> {
private:
  friend class Singleton;
  std::mutex m1;
  StructSubscriber() {}

public:
  void init_sub_list();
  void on_add_chunk(const Message &message);
  void on_file_renamed(const Message &message);
  void on_delete_entry(const Message &message);
};
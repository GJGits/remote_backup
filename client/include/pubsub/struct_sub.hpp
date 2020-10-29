#pragma once

#include <iostream>
#include <memory>
#include <mutex>

#include "../modules/module.hpp"
#include "../common/singleton.hpp"
#include "../filesystem/sync_structure.hpp"
#include "broker.hpp"

class StructSubscriber : public Singleton<StructSubscriber>, public Module {
private:
  friend class Singleton;
  std::mutex m1;
  StructSubscriber() {}

public:
  void start(const Message &message);
  void stop(const Message &message);
  void init_sub_list();
  void on_add_chunk(const Message &message);
  void on_file_renamed(const Message &message);
  void on_delete_entry(const Message &message);
};
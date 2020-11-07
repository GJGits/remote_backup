#pragma once

#include "../common/singleton.hpp"
#include "../filesystem/resource_guard.hpp"
#include "../filesystem/sync_structure.hpp"
#include "../modules/module.hpp"
#include <iostream>

class StructSubscriber : public Singleton<StructSubscriber>, public Module {
private:
  friend class Singleton;
  std::mutex m1;
  StructSubscriber();

public:
  ~StructSubscriber();
  void start();
  void stop();
  void init_sub_list();
  void on_add_entry(const Message &message);
  void on_delete_entry(const Message &message);
  void notify_news();
};
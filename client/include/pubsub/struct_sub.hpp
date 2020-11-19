#pragma once

#include "../modules/module.hpp"

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
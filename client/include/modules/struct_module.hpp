#pragma once

#include "../modules/module.hpp"

class StructModule : public Singleton<StructModule>, public Module {
private:
  friend class Singleton;
  std::mutex m1;
  StructModule();

public:
  ~StructModule();
  void start();
  void stop();
  void init_sub_list();
  void on_new_live(const Message &message);
  void on_delete_entry(const Message &message);
  void notify_news();
};
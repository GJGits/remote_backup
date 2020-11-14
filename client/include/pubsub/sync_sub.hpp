#pragma once

#include "../modules/module.hpp"

class SyncSubscriber : public Singleton<SyncSubscriber>, public Module {
private:
  friend class Singleton;
  SyncSubscriber();

public:
  ~SyncSubscriber();
  void start();
  void stop();
  void init_sub_list();
  void on_new_file(const Message &message);
  void on_file_deleted(const Message &message);
};
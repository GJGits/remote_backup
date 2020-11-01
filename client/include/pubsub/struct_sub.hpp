#pragma once

#include <iostream>
#include <memory>
#include <mutex>

#include "../common/singleton.hpp"
#include "../filesystem/sync_structure.hpp"
#include "../modules/module.hpp"
#include "broker.hpp"

class StructSubscriber : public Singleton<StructSubscriber>, public Module {
private:
  friend class Singleton;
  std::mutex m1;
  std::shared_ptr<SyncStructure> sync;
  std::shared_ptr<Broker> broker;
  StructSubscriber() {
    sync = SyncStructure::getInstance();
    broker = Broker::getInstance();
  }

public:
  void start(const Message &message);
  void stop(const Message &message);
  void init_sub_list();
  void on_add_entry(const Message &message);
  void on_delete_entry(const Message &message);
};
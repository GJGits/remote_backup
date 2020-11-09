#pragma once

#include "../exceptions/exceptions.hpp"
#include "../common/constants.hpp"
#include "../common/singleton.hpp"
#include "../common/utility.hpp"
#include "../common/duration.hpp"
#include "../pubsub/broker.hpp"
#include "../filesystem/resource_guard.hpp"
#include "../filesystem/entry_guard.hpp"
#include "../filesystem/sync_structure.hpp"
#include "../filesystem/file_entry.hpp"

class Module {

protected:
  bool running;
  std::shared_ptr<Broker> broker;

public:
  Module(): running{false} { broker = Broker::getInstance(); }
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void init_sub_list() = 0;
  void restart() {
    stop();
    start();
  }
};

class ModuleManager {
protected:
  std::mutex mu;
  std::shared_ptr<Broker> broker;
  std::vector<std::shared_ptr<Module>> modules;

public:
  ModuleManager() { broker = Broker::getInstance(); }
  virtual void add_module(const std::shared_ptr<Module> &module) = 0;
  virtual void start() = 0;
  virtual void close() = 0;
};
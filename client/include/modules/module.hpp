#pragma once
#include "../exceptions/exceptions.hpp"
#include "../pubsub/broker.hpp"
#include <memory>
#include <vector>

class Module {

protected:
  std::shared_ptr<Broker> broker;

public:
  Module() { broker = Broker::getInstance(); }
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void init_sub_list() = 0;
  void restart() {
    start();
    stop();
  }
};

class ModuleManager {
protected:
  std::vector<std::shared_ptr<Module>> modules;

public:
  ModuleManager() {}
  virtual void add_module(const std::shared_ptr<Module> &module) = 0;
  virtual void start() = 0;
  virtual void close() = 0;
};
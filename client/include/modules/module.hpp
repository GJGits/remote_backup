#pragma once

#include "../pubsub/broker.hpp"

class Module {

protected:
  std::shared_ptr<Broker> broker;

public:
  Module() {
    broker = Broker::getInstance();
    broker->subscribe(TOPIC::LOGGED_IN,
                      std::bind(&Module::start, this, std::placeholders::_1));
    broker->subscribe(TOPIC::LOGGED_OUT,
                      std::bind(&Module::stop, this, std::placeholders::_1));
    broker->subscribe(TOPIC::CLOSE,
                      std::bind(&Module::stop, this, std::placeholders::_1));
    broker->subscribe(TOPIC::RESTART,
                      std::bind(&Module::restart, this, std::placeholders::_1));
  }
  virtual void start(const Message &message) = 0;
  virtual void stop(const Message &message) = 0;
  virtual void init_sub_list() = 0;
  void restart(const Message &message) {
    start(message);
    stop(message);
  }
};

class ModuleManager {
protected:
std::shared_ptr<Broker> broker;
public:
  ModuleManager() {
    broker = Broker::getInstance();
    broker->subscribe(TOPIC::CLOSE, std::bind(&ModuleManager::close, this,
                                              std::placeholders::_1));
  }
  virtual void close(const Message &message) = 0;
};
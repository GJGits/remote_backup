#pragma once

#include "../common/singleton.hpp"
#include "../exceptions/exceptions.hpp"
#include "../filesystem/resource_guard.hpp"
#include "callback.hpp"
#include <functional>
#include <list>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <vector>

class Broker : public Singleton<Broker> {
private:
  friend class Singleton;
  SIGNAL last_signal;
  std::mutex mu_task;
  std::condition_variable cv_task;
  std::queue<std::function<void(void)>> tasks;
  std::unordered_map<SIGNAL, std::list<std::function<void(void)>>> sig_handlers;
  std::vector<std::thread> callers;
  bool is_running;
  std::unordered_map<TOPIC, std::list<Callback>> subs;
  Broker();

public:
  ~Broker();
  void register_signal(SIGNAL signal, const std::function<void(void)> &callback);
  void subscribe(TOPIC topic, PRIORITY priority,
                 const std::function<void(const Message &)> &callback);
  void publish(const Message &message);
  void signal(SIGNAL signal);
  void clear();
};

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
  std::mutex nm;
  std::condition_variable ncv;
  std::queue<std::function<void(void)>> tasks;
  std::vector<std::thread> callers;
  bool is_running;
  std::unordered_map<TOPIC, std::list<Callback>> subs;
  Broker();

public:
  ~Broker();
  void subscribe(TOPIC topic, PRIORITY priority,
                 const std::function<void(const Message &)> &callback);
  void publish(const Message &message);
  void clear();
};

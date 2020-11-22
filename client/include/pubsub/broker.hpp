#pragma once

#include <functional>
#include <list>
#include <tuple>
#include <unordered_map>
#include <queue>
#include <vector>
#include "../filesystem/resource_guard.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/singleton.hpp"
#include "message.hpp"


class Broker : public Singleton<Broker> {
private:
  friend class Singleton;
  std::mutex nm;
  std::condition_variable ncv;
  std::queue<std::function<void(void)>> tasks;
  std::vector<std::thread> callers;
  bool is_running;
  std::unordered_map<TOPIC, std::list<std::function<void(const Message &)>>>
      subs;
Broker();
public:
  ~Broker();
  void subscribe(const TOPIC &topic,
                 const std::function<void(const Message &)> &callback);
  void publish(const Message &message);
  void clear();
};

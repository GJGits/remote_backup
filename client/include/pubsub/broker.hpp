#pragma once

#include <functional>
#include <future>
#include <list>
#include <memory>
#include <tuple>
#include <unordered_map>

#include "../common/json.hpp"
#include "../common/singleton.hpp"
#include "message.hpp"
#include "topics.hpp"

using json = nlohmann::json;

class Broker : public Singleton<Broker> {
private:
  friend class Singleton;
  bool is_running;
  std::unordered_map<TOPIC, std::list<std::function<void(const Message &)>>>
      subs;
Broker() {}
public:
  
  void subscribe(const TOPIC &topic,
                 const std::function<void(const Message &)> &callback);
  void publish(const Message &message);
};
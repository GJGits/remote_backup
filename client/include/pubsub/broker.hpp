#pragma once

#include <future>
#include <functional>
#include <list>
#include <memory>
#include <tuple>
#include <unordered_map>

#include "../common/json.hpp"
#include "message.hpp"
#include "topics.hpp"

using json = nlohmann::json;

class Broker {
private:
  bool is_running;
  std::unordered_map<TOPIC, std::list<std::function<void(const Message &)>>>
      subs;
  static inline std::shared_ptr<Broker> instance{nullptr};

  Broker() {}

public:
  ~Broker() {}
  static std::shared_ptr<Broker> getInstance();
  void subscribe(const TOPIC &topic, const std::function<void(const Message &)> &callback);
  void publish(const Message &message);
};
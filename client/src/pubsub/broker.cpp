#include "../../include/pubsub/broker.hpp"

void Broker::subscribe(const TOPIC &topic,
                       const std::function<void(const Message &)> &callback) {
  if (subs.find(topic) == subs.end()) {
    subs.insert(std::pair{topic, std::list{callback}});
  } else {
    subs[topic].push_back(callback);
  }
}

void Broker::publish(const Message &message) {
  TOPIC topic = message.get_topic();
  if (subs.find(topic) != subs.end()) {
    for (auto const &call : subs[topic]) {
      std::async(std::launch::async, call, message);
    }
  }
}

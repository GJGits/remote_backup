#include "../../include/pubsub/broker.hpp"

Broker::Broker():is_running{true} {
  for(ssize_t i = 0; i < 4; i++) {

    callers.emplace_back([&](){
      while(is_running) {
        std::function<void(void)> fn;
        {
          std::unique_lock lk{nm};
          if (!tasks.empty()) {
            fn = std::move(tasks.front());
            tasks.pop();
          } else {
            ncv.wait(lk, [&](){return !tasks.empty() || !is_running;});
            continue;
          }
        }
        fn();
      }
    });
  }
}

Broker::~Broker() {
  is_running = false;
  for(ssize_t i = 0; i < 4; i++) {
    callers[i].join();
  }
}

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
      std::function<void(void)> fn = std::bind(call, message);
      push(fn);
    }
  }
}

void Broker::push(const std::function<void(void)> &pack) {
  std::unique_lock{nm};
  tasks.push(std::move(pack));
  ncv.notify_one();
}

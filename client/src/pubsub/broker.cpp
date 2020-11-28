#include "../../include/pubsub/broker.hpp"

Broker::Broker() : is_running{true} {
  for (ssize_t i = 0; i < 6; i++) {
    callers.emplace_back([this]() {
      while (is_running) {
        std::function<void(void)> fn;
        // sezione critica
        {
          std::unique_lock lk{nm};
          if (!tasks.empty()) {
            fn = tasks.front();
            tasks.pop();
          } else {
            ncv.wait(lk, [&]() { return !tasks.empty() || !is_running; });
            continue;
          }
        }
        // sezione non critica

        try {
          resource_guard guard{};
          fn();
        } catch (AuthFailed &ex) {
          std::clog << ex.what() << "\n";
          clear();
          publish(Message{TOPIC::AUTH_FAILED});
          // return;
        }

        catch (ConnectionNotAvaible &ex) {
          std::clog << ex.what() << "\n";
          clear();
          publish(Message{TOPIC::CONNECTION_LOST});
          // return;
        }

        catch (std::ifstream::failure &ex) {
          std::clog << "ifstream (broker) " << ex.what() << "\n";
          clear();
          publish(Message{TOPIC::EASY_EXCEPTION});
          // return;
        }

        catch (std::filesystem::filesystem_error &ex) {
          std::clog << ex.what() << "\n";
          clear();
          publish(Message{TOPIC::EASY_EXCEPTION});
          // return;
        }

        catch (...) {
          std::clog << "The impossible happened (Broker)!\n";
          clear();
          publish(Message{TOPIC::EASY_EXCEPTION});
          // return;
        }
      }
    });
  }
}

Broker::~Broker() {
  is_running = false;
  ncv.notify_all();
  for (ssize_t i = 0; i < 6; i++) {
    callers[i].join();
  }
  std::clog << "broker destroy\n";
}

void Broker::subscribe(TOPIC topic, PRIORITY priority,
                       const std::function<void(const Message &)> &callback) {
  std::unique_lock lk{nm};
  Callback cb{priority, callback};
  if (subs.find(topic) == subs.end()) {
    subs.insert(std::pair{topic, std::list{cb}});
  } else {
    subs[topic].push_back(cb);
    subs[topic].sort([&](const Callback &cb1, const Callback &cb2) {
      return cb2.get_priority() < cb1.get_priority();
    });
  }
}

void Broker::publish(const Message &message) {
  std::unique_lock lk{nm};
  TOPIC topic = message.get_topic();
  if (topic == TOPIC::EASY_EXCEPTION) {
    std::clog << "PUB_CLEAR\n";
  }
  if (subs.find(topic) != subs.end()) {
    for (auto const &call : subs[topic]) {
      std::function<void(void)> fn = std::bind(call.get_call(), message);
      tasks.push(fn);
      ncv.notify_one();
    }
  }
}

void Broker::clear() {
  std::unique_lock lk{nm};
  std::clog << "CLEAR\n";
  while (!tasks.empty()) {
    tasks.pop();
  }
}

#include "../../include/pubsub/broker.hpp"

Broker::Broker() : is_running{true} {
  for (ssize_t i = 0; i < 6; i++) {
    callers.emplace_back([&]() {
      while (is_running) {
        std::vector<std::function<void(const Message &)>> fns;
        Message mex;
        TOPIC topic;
        // sezione critica
        {
          std::unique_lock lk{nm};
          if (!tasks.empty()) {
            mex = tasks.front();
            tasks.pop();
          } else {
            // publish(Message{TOPIC::TRANSFER_COMPLETE});
            ncv.wait(lk, [&]() { return !tasks.empty() || !is_running; });
            continue;
          }
        }
        // sezione non critica

        try {
          resource_guard guard{};
          topic = mex.get_topic();
          if (subs.find(topic) != subs.end()) {
            for (auto const &call : subs[topic]) {
              call(mex);
            }
          }
        } catch (AuthFailed &ex) {
          std::clog << ex.what() << "\n";
          clear();
          publish(Message{TOPIC::AUTH_FAILED});
          return;
        }

        catch (ConnectionNotAvaible &ex) {
          std::clog << ex.what() << "\n";
          clear();
          publish(Message{TOPIC::CONNECTION_LOST});
          return;
        }

        catch (std::ifstream::failure &ex) {
          std::clog << ex.what() << "\n";
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
          return;
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

void Broker::subscribe(const TOPIC &topic,
                       const std::function<void(const Message &)> &callback) {
  std::unique_lock{nm};
  if (subs.find(topic) == subs.end()) {
    subs.insert(std::pair{topic, std::list{callback}});
  } else {
    subs[topic].push_back(callback);
  }
}

void Broker::publish(const Message &message) {
  std::unique_lock{nm};
  tasks.push(message);
  ncv.notify_one();
}

void Broker::clear() {
  std::unique_lock{nm};
  while (!tasks.empty()) {
    tasks.pop();
  }
}

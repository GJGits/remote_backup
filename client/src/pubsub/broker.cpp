#include "../../include/pubsub/broker.hpp"

Broker::Broker() : is_running{true} {
  for (ssize_t i = 0; i < 6; i++) {
    callers.emplace_back([&]() {
      while (is_running) {
        std::function<void(void)> fn;
        // sezione critica
        {
          std::unique_lock lk{nm};
          if (!tasks.empty()) {
            fn = std::move(tasks.front());
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
          publish(Message{TOPIC::AUTH_FAILED});
        }

        catch (ConnectionNotAvaible &ex) {
          std::clog << ex.what() << "\n";
          publish(Message{TOPIC::CONNECTION_LOST});
        }

        catch (std::ifstream::failure &ex) {
          std::clog << ex.what() << "\n";
          publish(Message{TOPIC::EASY_EXCEPTION});
        }

        catch (std::filesystem::filesystem_error &ex) {
          std::clog << ex.what() << "\n";
          publish(Message{TOPIC::EASY_EXCEPTION});
        }

        catch (...) {
          std::clog << "The impossible happened (Broker)!\n";
          publish(Message{TOPIC::EASY_EXCEPTION});
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



/*

  watcher  [t1]: create message 0x0001
  broker   [t1]: riceve, bind fn con message (0x0001)
  trhead x [tx]: chiama on_new_file(0x0001)
  trhead x [tx]: chiama 0x0001.get_content()
*/
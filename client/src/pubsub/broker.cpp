#include "../../include/pubsub/broker.hpp"

Broker::Broker() : is_running{true} {
  for (ssize_t i = 0; i < 4; i++) {
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
          fn();
        }

        catch (FileConfigNotValid &ex) {
          std::clog << ex.what() << "\n";
          std::ofstream o("./config/client-conf.json");
          json struct_ = {{"username", ""}, {"token", ""}};
          o << struct_ << "\n";
          publish(Message{TOPIC::LOGGED_OUT});
        }

        catch (SyncNotValid &ex) {
          std::clog << ex.what() << "\n";
          std::filesystem::create_directory("./sync");
          std::ofstream o("./config/client-struct.json");
          json struct_ = {{"entries", json::array()}, {"last_check", 0}};
          o << struct_ << "\n";
          publish(Message{TOPIC::RESTART});
        }

        catch (TmpNotValid &ex) {
          std::clog << ex.what() << "\n";
          std::filesystem::create_directory("./sync/.tmp");
          publish(Message{TOPIC::RESTART});
        }

        catch (BinNotValid &ex) {
          std::clog << ex.what() << "\n";
          std::filesystem::create_directory("./sync/.bin");
          publish(Message{TOPIC::RESTART});
        }

        catch (ConnectionNotAvaible &ex) {
          std::clog << ex.what() << "\n";
          publish(Message{TOPIC::LOGGED_OUT});
        }

        catch (std::ifstream::failure &ex) {
          std::clog << "Exception opening/reading/closing file\n";
        }

        catch (...) {
          std::clog << "The impossible happened!\n";
        }
      }
    });
  }
}

Broker::~Broker() {
  std::clog << "broker destroy\n";
  is_running = false;
  for (ssize_t i = 0; i < 4; i++) {
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

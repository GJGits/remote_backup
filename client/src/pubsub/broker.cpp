#include "../../include/pubsub/broker.hpp"

Broker::Broker()
    : last_signal{SIGNAL::TASKS_EMPTY}, is_running{true} {
  for (ssize_t i = 0; i < 6; i++) {
    callers.emplace_back([this]() {
      while (is_running) {
        std::function<void(void)> fn;
        // pop task in mutua esclusione
        {
          std::unique_lock lk{mu_task};
          if (!tasks.empty()) {
            fn = tasks.front();
            tasks.pop();
          } else {
            cv_task.wait(lk, [&]() { return !tasks.empty() || !is_running; });
            continue;
          }
        }
        // task in parallelo

        try {
          resource_guard guard{};
          fn();
        } catch (AuthFailed &ex) {
          std::clog << ex.what() << "\n";
          clear();
          signal(SIGNAL::AUTH_FAILED);
        }

        catch (ConnectionNotAvaible &ex) {
          std::clog << ex.what() << "\n";
          clear();
          signal(SIGNAL::CONNECTION_LOST);
        }

        catch (std::ifstream::failure &ex) {
          std::clog << "ifstream (broker) " << ex.what() << "\n";
          clear();
          signal(SIGNAL::EASY_EXCEPTION);
        }

        catch (std::filesystem::filesystem_error &ex) {
          std::clog << ex.what() << "\n";
          clear();
          signal(SIGNAL::EASY_EXCEPTION);
        }

        catch (...) {
          std::clog << "The impossible happened (Broker)!\n";
          clear();
          signal(SIGNAL::EASY_EXCEPTION);
        }
      }
    });
  }
}

Broker::~Broker() {
  is_running = false;
  cv_task.notify_all();
  for (ssize_t i = 0; i < 6; i++) {
    callers[i].join();
  }
  std::clog << "broker destroy\n";
}

void Broker::register_signal(SIGNAL signal,
                             const std::function<void(void)> &callback) {
  std::unique_lock lk{mu_task};
  if (sig_handlers.find(signal) == sig_handlers.end()) {
    sig_handlers[signal] = std::list{callback};
  } else {
    sig_handlers[signal].push_back(callback);
  }
}

void Broker::subscribe(TOPIC topic, PRIORITY priority,
                       const std::function<void(const Message &)> &callback) {
  std::unique_lock lk{mu_task};
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
  std::unique_lock lk{mu_task};
  TOPIC topic = message.get_topic();
  if (subs.find(topic) != subs.end()) {
    for (auto const &call : subs[topic]) {
      std::function<void(void)> fn = std::bind(call.get_call(), message);
      tasks.push(fn);
      cv_task.notify_one();
    }
  }
}

void Broker::signal(SIGNAL signal) {
  std::unique_lock lk{mu_task};
  // non voglio inviare due volte di seguito lo stesso segnale
  // ogni segnale inoltre va gestito in maniera sincrona
  if (signal != last_signal) {
    if (sig_handlers.find(signal) != sig_handlers.end()) {
      for (auto const &call : sig_handlers[signal]) {
        call();
      }
      last_signal = signal;
    }
  }
}

void Broker::clear() {
  std::unique_lock lk{mu_task};
  while (!tasks.empty()) {
    tasks.pop();
  }
}

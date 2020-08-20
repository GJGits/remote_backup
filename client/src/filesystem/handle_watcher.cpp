//
// Created by alessandra on 30/07/20.
//

#include "../../include/filesystem/handle_watcher.hpp"
#include "../../include/filesystem/linux_watcher.hpp"
#include "../../include/filesystem/sync_structure.hpp"

HandleWatcher *HandleWatcher::getInstance() {
  if (instance == nullptr) {
    instance = new HandleWatcher();
    instance->increase_counter(); // serve per prima scan
    instance->run_dispatcher();
  }
  return instance;
}

HandleWatcher::~HandleWatcher() {
  finish = true;
  dispatcher.join();
}

void HandleWatcher::run_dispatcher() {

  dispatcher = std::move(std::thread{[&]() {
    while (!finish) {

      std::unique_lock lk{m};

      if (!events.empty()) {

        Event event = events.front();
        events.pop();

        switch (event.getType()) {

        case EVENT_TYPE::CREATE:
          push_message({MESSAGE_CODE::UPLOAD,
                        std::vector<std::string>{event.getArgument1()}});
          break;

        case EVENT_TYPE::EXPAND:

          break;

        case EVENT_TYPE::MOVED:
          push_message({MESSAGE_CODE::MOVED, std::vector<std::string>{}});
          break;

        case EVENT_TYPE::DELETE:
          push_message({MESSAGE_CODE::REMOVE,
                        std::vector<std::string>{event.getArgument1()}});
          break;

        case EVENT_TYPE::RENAME:
          if (event.getArgument1().empty()) {
            push_message({MESSAGE_CODE::CREATE,
                          std::vector<std::string>{event.getArgument2()}});
          } else {
            push_message({MESSAGE_CODE::RENAME,
                          std::vector<std::string>{event.getArgument1(),
                                                   event.getArgument2()}});
          }

          break;

        case EVENT_TYPE::PRUNING:
          push_message({MESSAGE_CODE::PRUNING, std::vector<std::string>{}});
          break;

        default:
          break;
        }
      } else {
        cv.wait(lk, [&]() { return !events.empty() || finish; });
      }
    }
  }});
}

void HandleWatcher::push_event(const Event &event) {
  std::unique_lock lk{m};
  events.push(event);
  cv.notify_one();
}

void HandleWatcher::push_message(const Message &message) {
  std::unique_lock lk{mex_m};
  messages.push(message);
  mex_cv.notify_one();
}

Message HandleWatcher::pop_message(bool structure) {
  std::unique_lock lk{mex_m};
  if (messages.empty())
    mex_cv.wait(lk, [&]() { return !messages.empty() || finish; });
  if (!finish) {
    Message mex = messages.front();
    structure ? mex.set_struct(true) : mex.set_sync(true);
    if (mex.is_struct_handled() && mex.is_sync_handled())
      messages.pop();
    return mex;
  }
  return Message{MESSAGE_CODE::STOP, std::vector<std::string>{}};
}

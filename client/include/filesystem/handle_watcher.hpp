#pragma once

#include "../common/json.hpp"
#include "event.hpp"
#include "linux_watcher.hpp"
#include "message.hpp"
#include "sync_structure.hpp"
#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <memory>
#include <queue>
#include <regex>
#include <thread>
#include <vector>

using json = nlohmann::json;

class HandleWatcher {

private:
  inline static HandleWatcher *instance = nullptr;
  std::queue<Event> events;
  std::queue<std::shared_ptr<Message>> messages;
  std::thread dispatcher;
  std::mutex m;
  std::mutex mex_m;
  std::condition_variable cv, cv_mex;
  bool finish;

public:
  static HandleWatcher *getInstance() {
    if (instance == nullptr) {
      instance = new HandleWatcher{};
    }
    return instance;
  }

  ~HandleWatcher() {
    finish = true;
    dispatcher.join();
  }

  void run_dispatcher() {
    finish = false;
    dispatcher = std::move(std::thread{[&]() {
      while (!finish) {

        std::unique_lock lk{m};

        if (!events.empty()) {

          Event event = events.front();
          events.pop();

          switch (event.getType()) {

          case EVENT_TYPE::NEW_FILE:
            push_message(std::make_shared<SyncMessage>(
                SyncMessage{MESSAGE_CODE::UPLOAD,
                            std::vector<std::string>{event.getArgument1()}}));
            break;

          case EVENT_TYPE::NEW_FOLDER: {
            json folder;
            folder["name"] = event.getArgument1();
            push_message(std::make_shared<StructMessage>(
                StructMessage{MESSAGE_CODE::EXPAND, folder}));
          }

          break;

          case EVENT_TYPE::MOVED:
            push_message(std::make_shared<StructMessage>(
                StructMessage{MESSAGE_CODE::INCREASE_COUNTER}));
            break;

          case EVENT_TYPE::DELETE: {
            json file;
            file["name"] = event.getArgument1();
            push_message(std::make_shared<StructMessage>(
                StructMessage{MESSAGE_CODE::REMOVE, file}));
            push_message(std::make_shared<SyncMessage>(
                SyncMessage{MESSAGE_CODE::REMOVE,
                            std::vector<std::string>{event.getArgument1()}}));

          }

          break;

          case EVENT_TYPE::RENAME:
            if (event.getArgument1().empty()) {
              push_message(std::make_shared<SyncMessage>(
                  SyncMessage{MESSAGE_CODE::UPLOAD,
                              std::vector<std::string>{event.getArgument2()}}));
            } else {
              json entry;
              entry["old"] = event.getArgument1();
              entry["new"] = event.getArgument2();
              push_message(std::make_shared<StructMessage>(
                  StructMessage{MESSAGE_CODE::UPDATE_PATH, entry}));

              push_message(std::make_shared<SyncMessage>(
                  SyncMessage{MESSAGE_CODE::UPDATE_PATH,
                              std::vector<std::string>{event.getArgument1(),
                                                       event.getArgument2()}}));
            }
            break;

          case EVENT_TYPE::TIME_OUT:
            push_message(std::make_shared<StructMessage>(
                StructMessage{MESSAGE_CODE::PRUNING}));
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

  void push_event(const Event &event) {
    std::unique_lock lk{m};
    events.push(event);
    cv.notify_one();
  }

  void push_message(std::shared_ptr<Message> message) {
    std::unique_lock lk{mex_m};
    messages.push(message);
    cv_mex.notify_all();
  }

  std::shared_ptr<Message> pop_message(int type) {
    std::unique_lock lk{mex_m};

    if (messages.empty() || messages.front()->getType() != type)
      cv_mex.wait(lk, [&]() {
        return (!messages.empty() && messages.front()->getType() == type) ||
               finish;
      });
    if (!messages.empty()) {
      std::shared_ptr<Message> message = messages.front();
      messages.pop();
      return message;
    }
    return std::make_shared<SignalMessage>(SignalMessage{MESSAGE_CODE::STOP});
  }
};

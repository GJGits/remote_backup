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

          case EVENT_TYPE::CREATE:
            // push_sync_message(
            // SyncMessage{MESSAGE_CODE::UPLOAD,
            //         std::vector<std::string>{event.getArgument1()}});
            break;

          case EVENT_TYPE::EXPAND:
            // push_sync_message(SyncMessage{MESSAGE_CODE::BULK_UPLOAD,
            //                          std::vector<std::string>{event.getArgument1()}});

            break;

          case EVENT_TYPE::MOVED:
            // push_struct_message(
            // StructMessage{MESSAGE_CODE::FILE_MOVED,
            // std::vector<std::string>{}});
            break;

          case EVENT_TYPE::DELETE:
            //  push_sync_message(
            //    SyncMessage{MESSAGE_CODE::REMOVE,
            //              std::vector<std::string>{event.getArgument1()}});
            break;

          case EVENT_TYPE::RENAME:
            if (event.getArgument1().empty()) {
              // push_sync_message(SyncMessage{
              //   MESSAGE_CODE::UPLOAD,
              // std::vector<std::string>{event.getArgument2()}});
            } else {
              // push_sync_message(SyncMessage{
              // MESSAGE_CODE::UPDATE_PATH,
              // std::vector<std::string>{event.getArgument1(),
              //                      event.getArgument2()}});
            }

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

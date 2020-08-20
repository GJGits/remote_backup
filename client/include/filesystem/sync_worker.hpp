#pragma once

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "handle_watcher.hpp"
#include "http_req.hpp"
#include "message.hpp"

class SyncWorker {
private:
  std::vector<std::string> loopback_filter_cache;
  std::queue<HTTPReq> down_reqs;
  std::queue<HTTPReq> up_reqs;
  std::mutex cache_m, down_m, up_m;
  std::condition_variable cache_cv, down_cv, up_cv;
  std::thread sync_worker;
  std::vector<std::thread> down_workers, up_workers;
  bool finish;
  inline static SyncWorker *instance = nullptr;

  SyncWorker() {}

public:
  ~SyncWorker() {
    finish = true;
    sync_worker.join();
    for (size_t i; i < down_workers.size(); i++) {
      down_workers[i].join();
    }
    for (size_t i; i < up_workers.size(); i++) {
      up_workers[i].join();
    }
  }

  static SyncWorker *getInstance() {
    if (instance == nullptr) {
      instance = new SyncWorker{};
    }
    return instance;
  }

  void run_sync_worker() {
    HandleWatcher *handle_watcher = HandleWatcher::getInstance();
    sync_worker = std::move(std::thread{[&]() {
      while (!finish) {
        Message mex = handle_watcher->pop_message(false);
        std::clog << "Message handled in sync: " << mex.is_sync_handled() << "\n";
        // todo: sync job here
      }
    }});
  }

  void run_down_workers() {
    down_workers.emplace_back([&]() {
      while (!finish) {
        // todo: down job here
      }
    });
  }

  void run_up_workers() {
    up_workers.emplace_back([&]() {
      while (!finish) {
        // todo: up job here
      }
    });
  }

  void scan() {
    HandleWatcher *handle_watcher = HandleWatcher::getInstance();
    Message m1 {MESSAGE_CODE::BULK_INSERT, std::vector<std::string>{"./sync"}};
    Message m2 {MESSAGE_CODE::PRUNING, std::vector<std::string>{}};
    m1.set_sync(true);
    m2.set_sync(true);
    handle_watcher->push_message(m1);
    handle_watcher->push_message(m2);
  }
};
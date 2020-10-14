#pragma once

#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "../common/json.hpp"
#include "../common/singleton.hpp"
#include "../common/utility.hpp"
#include "../filesystem/file_entry.hpp"
#include "../filesystem/sync_structure.hpp"
#include "../http/rest_client.hpp"
#include "broker.hpp"

#define MAX_SYNC_SIZE 2147483648 // 2 GB

using json = nlohmann::json;

class SyncSubscriber : public Singleton<SyncSubscriber> {
private:
  friend class Singleton;
  std::vector<std::thread> down_workers;
  std::queue<json> down_tasks;
  std::mutex m;
  std::condition_variable cv;
  bool running;
  size_t dir_size;
  SyncSubscriber() : running{false}, dir_size{0} {}

public:
  ~SyncSubscriber() {
    running = false;
    for (size_t i = 0; i < down_workers.size(); i++) {
      down_workers[i].join();
    }
  }
  void start(const Message &message);
  void stop(const Message &message);
  void init_sub_list();
  void init_workers();
  void on_new_file(const Message &message);
  void on_new_folder(const Message &message);
  void on_file_renamed(const Message &message);
  void on_file_deleted(const Message &message);
  void remote_check();
  void increment_size(size_t size);
  void compute_new_size();
  void push(const json &task);
  void restore_files();
};
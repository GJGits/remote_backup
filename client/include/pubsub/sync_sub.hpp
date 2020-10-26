#pragma once

#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../common/json.hpp"
#include "../common/singleton.hpp"
#include "../common/utility.hpp"
#include "../filesystem/file_entry.hpp"
#include "../filesystem/sync_structure.hpp"
#include "../http/rest_client.hpp"
#include "broker.hpp"

using json = nlohmann::json;

class SyncSubscriber : public Singleton<SyncSubscriber> {
private:
  friend class Singleton;
  std::vector<std::thread> down_workers;
  std::queue<json> down_tasks;
  std::mutex m;
  std::condition_variable cv;
  bool running;
  SyncSubscriber() : running{false} {std::clog << "Sync module init...\n";}

public:
  ~SyncSubscriber() {
    running = false;
    for (size_t i = 0; i < down_workers.size(); i++) {
      down_workers[i].join();
    }
    std::clog << "Sync module destroy...\n";
  }
  void start(const Message &message);
  void stop(const Message &message);
  void init_sub_list();
  void init_workers();
  void on_new_file(const Message &message);
  void on_file_renamed(const Message &message);
  void on_file_deleted(const Message &message);
  void collect_unfinished_transfers(std::unordered_map<std::string, json> &changes);
  void collect_local_changes(std::unordered_map<std::string, json> &changes);
  void collect_remote_changes(std::unordered_map<std::string, json> &changes);
  void run_corrections(std::unordered_map<std::string, json> &changes);
  void push(const json &task);
  void restore_files();
};
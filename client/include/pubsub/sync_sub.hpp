#pragma once

#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "../common/json.hpp"
#include "../common/singleton.hpp"
#include "../common/utility.hpp"
#include "../exceptions/exceptions.hpp"
#include "../filesystem/file_entry.hpp"
#include "../filesystem/sync_structure.hpp"
#include "../http/rest_client.hpp"
#include "../modules/module.hpp"

using json = nlohmann::json;

class SyncSubscriber : public Singleton<SyncSubscriber>, public Module {
private:
  friend class Singleton;
  std::unordered_map<std::string, json> changes;
  std::vector<std::thread> down_workers;
  std::queue<json> down_tasks;
  std::mutex m;
  std::condition_variable cv;
  bool running;
  std::shared_ptr<RestClient> rest_client;
  std::shared_ptr<SyncStructure> syn;
  SyncSubscriber() : running{false} {
    std::clog << "Sync module init...\n";
    syn = SyncStructure::getInstance();
    rest_client = RestClient::getInstance();
    std::clog << "sync services init\n";
  }

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
  void on_file_deleted(const Message &message);
  void push(const json &task);
  void restore_files();
};
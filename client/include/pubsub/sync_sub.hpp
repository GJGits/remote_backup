#pragma once

#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <queue>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "../common/json.hpp"
#include "../common/singleton.hpp"
#include "../common/utility.hpp"
#include "../filesystem/file_entry.hpp"
#include "../filesystem/sync_structure.hpp"
#include "../modules/module.hpp"

using json = nlohmann::json;

class SyncSubscriber : public Singleton<SyncSubscriber>, public Module {
private:
  friend class Singleton;
  std::unordered_map<std::string, json> changes;
  std::vector<std::thread> down_workers;
  std::queue<std::shared_ptr<FileEntry>> down_tasks;
  std::mutex m;
  std::condition_variable cv;
  bool running;
  SyncSubscriber();

public:
  ~SyncSubscriber();
  void start();
  void stop();
  void init_sub_list();
  void init_workers();
  void on_new_file(const Message &message);
  void on_file_deleted(const Message &message);
  void push(const std::shared_ptr<FileEntry> &task);
  void restore_files();
};
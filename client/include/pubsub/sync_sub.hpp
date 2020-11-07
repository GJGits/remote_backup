#pragma once

#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <queue>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "../filesystem/resource_guard.hpp"
#include "../common/singleton.hpp"
#include "../common/utility.hpp"
#include "../filesystem/file_entry.hpp"
#include "../filesystem/sync_structure.hpp"
#include "../modules/module.hpp"

class SyncSubscriber : public Singleton<SyncSubscriber>, public Module {
private:
  friend class Singleton;
  bool running;
  SyncSubscriber();

public:
  ~SyncSubscriber();
  void start();
  void stop();
  void init_sub_list();
  void on_new_file(const Message &message);
  void on_file_deleted(const Message &message);
  void restore_files();
};
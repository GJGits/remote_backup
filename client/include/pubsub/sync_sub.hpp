#pragma once

#include <filesystem>
#include <iostream>
#include <memory>

#include "../common/json.hpp"
#include "../filesystem/sync_structure.hpp"
#include "../filesystem/file_entry.hpp"
#include "../http/rest_client.hpp"
#include "broker.hpp"

#define MAX_SYNC_SIZE 2147483648 // 2 GB

using json = nlohmann::json;

class SyncSubscriber {
private:
  size_t dir_size;
  static inline std::shared_ptr<SyncSubscriber> instance{nullptr};
  SyncSubscriber():dir_size{0} {}
public:
  static std::shared_ptr<SyncSubscriber> getInstance();
  void init();
  void on_new_file(const Message &message);
  void on_new_folder(const Message &message);
  void on_file_renamed(const Message &message);
  void on_file_deleted(const Message &message);
  void remote_check();
  void increment_size(size_t size);
  void compute_new_size();
};
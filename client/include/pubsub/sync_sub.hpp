#pragma once

#include "../modules/module.hpp"

class SyncSubscriber : public Singleton<SyncSubscriber>, public Module {
private:
  friend class Singleton;
  std::mutex mx;
  size_t remote_transfer_count;
  SyncSubscriber();
  // void start_remote_sync();
  void end_remote_sync();

public:
  ~SyncSubscriber();
  void start();
  void stop();
  void init_sub_list();
  // void on_new_file(const Message &message);
  // void on_file_deleted(const Message &message);
  void new_from_local(const Message &message);
  void new_from_remote(const Message &message);
  void delete_from_local(const Message &message);
  void delete_from_remote(const Message &message);
};
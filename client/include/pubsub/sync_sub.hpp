#pragma once

#include "../modules/module.hpp"

class SyncSubscriber : public Singleton<SyncSubscriber>, public Module {
private:
  friend class Singleton;
  SyncSubscriber();
  void new_from_local(const std::shared_ptr<FileEntry> &fentry);
  void new_from_remote(const std::shared_ptr<FileEntry> &fentry);
  void delete_from_local(const std::shared_ptr<FileEntry> &fentry);
  void delete_from_remote(const std::shared_ptr<FileEntry> &fentry);

public:
  ~SyncSubscriber();
  void start();
  void stop();
  void init_sub_list();
  void on_new_file(const Message &message);
  void on_file_deleted(const Message &message);
};
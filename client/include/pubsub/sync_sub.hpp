#pragma once

#include <iostream>
#include <memory>

#include "broker.hpp"

class SyncSubscriber {
private:
  static inline std::shared_ptr<SyncSubscriber> instance{nullptr};
  SyncSubscriber(){}
public:
  static std::shared_ptr<SyncSubscriber> getInstance();
  void init();
  void on_new_file(const Message &message);
  void on_new_folder(const Message &message);
  void on_file_renamed(const Message &message);
  void on_file_modified(const Message &message);
  void on_file_deleted(const Message &message);
  void on_bulk_delete(const Message &message);
};
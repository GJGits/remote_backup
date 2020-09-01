#include "../../include/pubsub/sync_sub.hpp"

std::shared_ptr<SyncSubscriber> SyncSubscriber::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<SyncSubscriber>(new SyncSubscriber{});
  }
  return instance;
}

void SyncSubscriber::init() {
  std::shared_ptr<Broker> broker = Broker::getInstance();
  broker->subscribe(TOPIC::NEW_FILE,
                    std::bind(&SyncSubscriber::on_new_file, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::NEW_FOLDER,
                    std::bind(&SyncSubscriber::on_new_folder, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::FILE_RENAMED,
                    std::bind(&SyncSubscriber::on_file_renamed, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::FILE_MODIFIED,
                    std::bind(&SyncSubscriber::on_file_modified, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::FILE_DELETED,
                    std::bind(&SyncSubscriber::on_file_deleted, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::BULK_DELETE,
                    std::bind(&SyncSubscriber::on_bulk_delete, instance.get(),
                              std::placeholders::_1));
}

void SyncSubscriber::on_new_file(const Message &message) {
  std::clog << "New file\n";
}
void SyncSubscriber::on_new_folder(const Message &message) {
  std::clog << "New folder\n";
}
void SyncSubscriber::on_file_renamed(const Message &message) {
  std::clog << "New file_renamed\n";
}
void SyncSubscriber::on_file_modified(const Message &message) {
  std::clog << "New file_modified\n";
}
void SyncSubscriber::on_file_deleted(const Message &message) {
  std::clog << "New file_deleted\n";
}
void SyncSubscriber::on_bulk_delete(const Message &message) {
  std::clog << "bulk_delete\n";
}
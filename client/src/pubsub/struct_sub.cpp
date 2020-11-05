#include "../../include/pubsub/struct_sub.hpp"

StructSubscriber::StructSubscriber() { std::clog << "Struct module init...\n"; }

void StructSubscriber::init_sub_list() {
  broker->subscribe(TOPIC::ADD_ENTRY,
                    std::bind(&StructSubscriber::on_add_entry, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::REMOVE_ENTRY,
                    std::bind(&StructSubscriber::on_delete_entry,
                              instance.get(), std::placeholders::_1));
}

void StructSubscriber::start() {
  std::clog << "Struct module start...\n";
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->restore();
  sync->update_from_fs();
  sync->update_from_remote();
  notify_news();
}

void StructSubscriber::stop() {
  std::clog << "Struct module stop...\n";
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->store();
}

void StructSubscriber::on_add_entry(const Message &message) {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  std::shared_ptr<FileEntry> entry = message.get_content();
  sync->add_entry(entry);
}

void StructSubscriber::on_delete_entry(const Message &message) {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->remove_entry(message.get_content());
}

void StructSubscriber::notify_news() {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  for (const auto &entry : sync->get_entries()) {
    if (entry->get_status() == entry_status::new_) {
      broker->publish(Message{TOPIC::NEW_FILE, entry});
    }
    if (entry->get_status() == entry_status::delete_) {
      broker->publish(Message{TOPIC::FILE_DELETED, entry});
    }
  }
}

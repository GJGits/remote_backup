#include "../../include/pubsub/struct_sub.hpp"

StructSubscriber::StructSubscriber() { std::clog << "Struct module init...\n"; }

StructSubscriber::~StructSubscriber() {
  std::clog << "Struct module destroy...\n";
}

void StructSubscriber::init_sub_list() {
  broker = Broker::getInstance();
  broker->subscribe(TOPIC::ADD_ENTRY,
                    std::bind(&StructSubscriber::on_add_entry, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::REMOVE_ENTRY,
                    std::bind(&StructSubscriber::on_delete_entry,
                              instance.get(), std::placeholders::_1));
}

void StructSubscriber::start() {
  if (!running) {
    running = true;
    std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
    sync->restore();
    sync->update_from_fs();
    sync->update_from_remote();
    notify_news();
    std::clog << "Struct module start...\n";
  }
}

void StructSubscriber::stop() {
  if (running) {
    running = false;
    std::unique_lock lock{m1};
    std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
    sync->store();
    std::clog << "Struct module stop...\n";
  }
}

void StructSubscriber::on_add_entry(const Message &message) {
  DurationLogger log{"ADD_ENTRY"};
  /*
  std::unique_lock lock{m1};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  std::shared_ptr<FileEntry> entry = message.get_content();
  sync->add_entry(entry);
  */
}

void StructSubscriber::on_delete_entry(const Message &message) {
  DurationLogger log{"DELETE_ENTRY"};
  /*
  std::unique_lock lock{m1};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  std::shared_ptr<FileEntry> entry = message.get_content();
  std::clog << "count in struct: " << entry.use_count() << "\n";
  sync->remove_entry(entry);
  */
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

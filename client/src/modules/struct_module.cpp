#include "../../include/modules/struct_module.hpp"

StructModule::StructModule() { std::clog << "Struct module init...\n"; }

StructModule::~StructModule() { std::clog << "Struct module destroy...\n"; }

void StructModule::init_sub_list() {
  broker = Broker::getInstance();
  broker->subscribe(TOPIC::ADD_ENTRY, PRIORITY::LOW,
                    std::bind(&StructModule::on_add_entry, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::REMOVE_ENTRY, PRIORITY::LOW,
                    std::bind(&StructModule::on_delete_entry, instance.get(),
                              std::placeholders::_1));
}

void StructModule::start() {
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

void StructModule::stop() {
  if (running) {
    running = false;
    std::unique_lock lock{m1};
    std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
    sync->store();
    std::clog << "Struct module stop...\n";
  }
}

void StructModule::on_add_entry(const Message &message) {
  std::unique_lock lock{m1};
  DurationLogger log{"ADD_ENTRY"};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  std::shared_ptr<FileEntry> entry = message.get_content();
  sync->add_entry(entry);
}

void StructModule::on_delete_entry(const Message &message) {
  std::unique_lock lock{m1};
  DurationLogger log{"DELETE_ENTRY"};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  std::shared_ptr<FileEntry> entry = message.get_content();
  sync->remove_entry(entry);
}

void StructModule::notify_news() {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  if (sync->get_remote_news() > 0)
    broker->publish(Message{TOPIC::INIT_SERVER_SYNC});
  for (const auto &entry : sync->get_entries()) {
    if (entry->get_status() == entry_status::new_) {
      broker->publish(Message{TOPIC::NEW_FILE, entry});
    }
    if (entry->get_status() == entry_status::delete_) {
      broker->publish(Message{TOPIC::FILE_DELETED, entry});
    }
  }
}
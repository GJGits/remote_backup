#include "../../include/modules/struct_module.hpp"

StructModule::StructModule() { std::clog << "Struct module init...\n"; }

StructModule::~StructModule() { std::clog << "Struct module destroy...\n"; }

void StructModule::init_sub_list() {
  broker = Broker::getInstance();
  broker->subscribe(TOPIC::NEW_LIVE, PRIORITY::HIGH,
                    std::bind(&StructModule::on_new_live, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::REMOVE_ENTRY, PRIORITY::HIGH,
                    std::bind(&StructModule::on_delete_entry, instance.get(),
                              std::placeholders::_1));
}

void StructModule::start() {
  std::unique_lock lock{m1};
  if (!running) {
    running = true;
    std::unique_lock lock{m1};
    std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
    sync->restore();
    sync->update_from_remote();
    sync->update_from_fs();
    notify_news();
    std::clog << "Struct module start...\n";
  }
}

void StructModule::stop() {
  std::unique_lock lock{m1};
  if (running) {
    running = false;
    std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
    sync->store();
    std::clog << "Struct module stop...\n";
  }
}

void StructModule::on_new_live(const Message &message) {
  std::unique_lock lock{m1};
  DurationLogger log{"NEW_LIVE"};
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

  if (sync->get_remote_news() == 0) {
    broker->publish(Message{TOPIC::FINISH_SERVER_SYNC});
  }
  for (const auto &entry : sync->get_entries()) {
    if (entry->get_status() == entry_status::new_ &&
        entry->get_producer() == entry_producer::local) {
      broker->publish(Message{TOPIC::NEW_OFFLINE, entry});
    }
    if (entry->get_status() == entry_status::new_ &&
        entry->get_producer() == entry_producer::server) {
      broker->publish(Message{TOPIC::NEW_SERVER, entry});
    }
    if (entry->get_status() == entry_status::delete_ &&
        entry->get_producer() == entry_producer::local) {
      broker->publish(Message{TOPIC::DELETE_OFFLINE, entry});
    }
    if (entry->get_status() == entry_status::delete_ &&
        entry->get_producer() == entry_producer::server) {
      broker->publish(Message{TOPIC::DELETE_SERVER, entry});
    }
  }
}
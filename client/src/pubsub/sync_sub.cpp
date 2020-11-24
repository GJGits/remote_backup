#include "../../include/pubsub/sync_sub.hpp"

SyncSubscriber::SyncSubscriber() : remote_transfer_count{0} {
  std::clog << "Sync module init...\n";
}

SyncSubscriber::~SyncSubscriber() {
  running = false;
  std::clog << "Sync module destroy...\n";
}

void SyncSubscriber::init_sub_list() {
  broker = Broker::getInstance();
  broker->subscribe(TOPIC::NEW_FILE, PRIORITY::LOW,
                    std::bind(&SyncSubscriber::on_new_file, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::FILE_DELETED, PRIORITY::LOW,
                    std::bind(&SyncSubscriber::on_file_deleted, instance.get(),
                              std::placeholders::_1));
}

void SyncSubscriber::start() {
  running = true;
  std::clog << "sync module started...\n";
}

void SyncSubscriber::stop() {
  running = false;
  std::clog << "sync module stop...\n";
}

void SyncSubscriber::on_new_file(const Message &message) {
  DurationLogger logger{"NEW_FILE"};
  std::shared_ptr<FileEntry> fentry = message.get_content();
  entry_guard eguard{fentry};
  if (fentry->get_producer() == entry_producer::local && fentry->has_chunk()) {
    new_from_local(fentry);
  }
  if (fentry->get_producer() == entry_producer::server) {
    new_from_remote(fentry);
  }
  fentry->set_status(entry_status::synced);
  broker->publish(Message{TOPIC::ADD_ENTRY, fentry});
}

void SyncSubscriber::new_from_local(const std::shared_ptr<FileEntry> &fentry) {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  while (fentry->has_chunk()) {
    std::tuple<std::shared_ptr<char[]>, size_t> chunk = fentry->next_chunk();
    rest_client->post_chunk(chunk, fentry->to_string());
  }
}

void SyncSubscriber::new_from_remote(const std::shared_ptr<FileEntry> &fentry) {
  start_remote_sync();
  if(std::filesystem::exists(fentry->get_path()))
  	std::remove(fentry->get_path().c_str());
  while (fentry->has_chunk()) {
    fentry->retrieve_chunk();
  }
  end_remote_sync();
}

void SyncSubscriber::on_file_deleted(const Message &message) {
  DurationLogger logger{"FILE_DELETED"};
  std::shared_ptr<FileEntry> fentry = message.get_content();
  if (fentry->get_producer() == entry_producer::local) {
    delete_from_local(fentry);
  }
  if (fentry->get_producer() == entry_producer::server &&
      std::filesystem::exists(fentry->get_path())) {
    delete_from_remote(fentry);
  }
  fentry->set_status(entry_status::synced);
  broker->publish(Message{TOPIC::REMOVE_ENTRY, fentry});
}

void SyncSubscriber::delete_from_local(
    const std::shared_ptr<FileEntry> &fentry) {
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  rest_client->delete_file(fentry->get_path());
}

void SyncSubscriber::delete_from_remote(
    const std::shared_ptr<FileEntry> &fentry) {
  start_remote_sync();
  if (std::filesystem::exists(fentry->get_path())) {
    std::remove(fentry->get_path().c_str());
    std::filesystem::path parent_path =
        std::filesystem::path(fentry->get_path()).parent_path();
    if (std::filesystem::is_empty(parent_path)) {
      std::filesystem::remove_all(parent_path);
    }
  }
  end_remote_sync();
}

void SyncSubscriber::start_remote_sync() {
  std::unique_lock lk{mx};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  if (remote_transfer_count == 0) {
    broker->publish(Message{TOPIC::INIT_SERVER_SYNC});
  }
}

void SyncSubscriber::end_remote_sync() {
  std::unique_lock lk{mx};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  remote_transfer_count++;
  if (sync->get_remote_news() == remote_transfer_count) {
    sync->reset_remote_news();
    broker->publish(Message{TOPIC::FINISH_SERVER_SYNC});
  }
}

#include "../../include/pubsub/sync_sub.hpp"

SyncSubscriber::SyncSubscriber() { std::clog << "Sync module init...\n"; }

SyncSubscriber::~SyncSubscriber() {
  running = false;
  std::clog << "Sync module destroy...\n";
}

void SyncSubscriber::init_sub_list() {
  broker = Broker::getInstance();
  broker->subscribe(TOPIC::NEW_FILE,
                    std::bind(&SyncSubscriber::on_new_file, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::FILE_DELETED,
                    std::bind(&SyncSubscriber::on_file_deleted, instance.get(),
                              std::placeholders::_1));
}

void SyncSubscriber::start() {
  std::clog << "sync module started...\n";
  running = true;
}

void SyncSubscriber::stop() {
  std::clog << "sync module stop...\n";
  running = false;
}

void SyncSubscriber::on_new_file(const Message &message) {
  DurationLogger logger{"NEW_FILE"};
  std::shared_ptr<FileEntry> fentry = message.get_content();
  entry_guard eguard{fentry};
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  if (fentry->get_producer() == entry_producer::local && fentry->has_chunk()) {
    while (fentry->has_chunk()) {
      std::tuple<std::shared_ptr<char[]>, size_t> chunk = fentry->next_chunk();
      rest_client->post_chunk(chunk, fentry->to_string());
    }
    broker->publish(Message{TOPIC::ADD_ENTRY, fentry});
  }
  if (fentry->get_producer() == entry_producer::server) {
    while (fentry->has_chunk()) {
      fentry->retrieve_chunk();
    }
    std::filesystem::path new_path{fentry->get_path()};
    std::filesystem::path tmp_path{TMP_PATH + std::string{"/"} +
                                   macaron::Base64::Encode(fentry->get_path()) +
                                   std::string{".out"}};
    std::filesystem::create_directories(new_path.parent_path().string());
    std::filesystem::rename(tmp_path, new_path);
    std::filesystem::remove(tmp_path);
  }
  fentry->set_status(entry_status::synced);
  // broker->publish(Message{TOPIC::TRANSFER_COMPLETE, fentry});
}

void SyncSubscriber::on_file_deleted(const Message &message) {
  DurationLogger logger{"FILE_DELETED"};
  std::shared_ptr<FileEntry> fentry = message.get_content();
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  if (fentry->get_producer() == entry_producer::local) {
    rest_client->delete_file(fentry->get_path());
  }
  if (fentry->get_producer() == entry_producer::server &&
      std::filesystem::exists(fentry->get_path())) {
    std::string new_path{BIN_PATH + std::string{"/a"}};
    std::filesystem::rename(fentry->get_path(), new_path);
    std::remove(new_path.c_str());
    std::filesystem::path parent_path =
        std::filesystem::path(fentry->get_path()).parent_path();
    if (std::filesystem::is_empty(parent_path)) {
      std::filesystem::remove_all(parent_path);
    }
  }
  fentry->set_status(entry_status::synced);
  broker->publish(Message{TOPIC::REMOVE_ENTRY, fentry});
  broker->publish(Message{TOPIC::TRANSFER_COMPLETE, fentry});
}

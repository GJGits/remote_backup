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
    broker->publish(Message{TOPIC::ADD_ENTRY, fentry});
    while (fentry->has_chunk()) {
      std::tuple<std::shared_ptr<char[]>, size_t> chunk = fentry->next_chunk();
      rest_client->post_chunk(chunk, fentry->to_string());
    }
  }
  if (fentry->get_producer() == entry_producer::server) {
    while (fentry->has_chunk()) {
      fentry->retrieve_chunk();
    }
    restore_files(fentry);
  }
  fentry->set_status(entry_status::synced);
  broker->publish(Message{TOPIC::TRANSFER_COMPLETE, fentry});
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
  }
  fentry->set_status(entry_status::synced);
  broker->publish(Message{TOPIC::REMOVE_ENTRY, fentry});
  broker->publish(Message{TOPIC::TRANSFER_COMPLETE, fentry});
}

void SyncSubscriber::restore_files(const std::shared_ptr<FileEntry> &fentry) {
std::string p {std::string{TMP_PATH} + std::string{"/"} + macaron::Base64::Encode(fentry->get_path())};
    std::clog << p << "\n";

      std::vector<std::string> chunks{};
      for (const auto &entry : std::filesystem::directory_iterator(p)) {
        if (entry.is_regular_file()) {
          chunks.push_back(entry.path().string());
        }
      }

      std::sort(chunks.begin(), chunks.end(),
                [&](const std::string &c1, const std::string &c2) {
                  return (c1.size() != c2.size()) ? c1.size() < c2.size()
                                                  : c1.compare(c2) < 0;
                });
      for(size_t i=0 ; i < chunks.size() ; i++){
          std::clog << chunks[i] << "\n";
      }

      std::string temp_path = p + ".out";
      std::ofstream out{temp_path, std::ios::binary};
      for (size_t i = 0; i < chunks.size(); i++) {
        std::ifstream in{chunks[i], std::ios::binary};
        int fsize = std::filesystem::file_size(chunks[i]);
        char buff[fsize];
        memset(buff, '\0', fsize);
        in.read(buff, fsize);
        out.write(buff, fsize);
      }
      out.close();
      std::filesystem::path folders{fentry->get_path()};
      std::filesystem::create_directories(folders.parent_path().string());
      std::filesystem::rename(temp_path, fentry->get_path());
      std::filesystem::remove_all(p);

}

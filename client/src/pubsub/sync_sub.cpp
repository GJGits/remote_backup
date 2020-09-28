#include "../../include/pubsub/sync_sub.hpp"

std::shared_ptr<SyncSubscriber> SyncSubscriber::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<SyncSubscriber>(new SyncSubscriber{});
    instance->compute_new_size();
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
                    std::bind(&SyncSubscriber::on_new_file, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::FILE_DELETED,
                    std::bind(&SyncSubscriber::on_file_deleted, instance.get(),
                              std::placeholders::_1));
}

void SyncSubscriber::compute_new_size() {
  for (auto &p : std::filesystem::recursive_directory_iterator("./sync")) {
    if (p.is_regular_file()) {
      increment_size(std::filesystem::file_size(p.path().string()));
    }
  }
}

void SyncSubscriber::increment_size(size_t size) { dir_size += size; }

void SyncSubscriber::on_new_file(const Message &message) {
  DurationLogger logger{"NEW_FILE"};
  json content = message.get_content();
  std::string file_path = content["path"];
  if (std::filesystem::exists(file_path) &&
      !std::filesystem::is_empty(file_path)) {
    std::shared_ptr<Broker> broker = Broker::getInstance();
    std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
    increment_size(std::filesystem::file_size(file_path));
    if (dir_size < MAX_SYNC_SIZE) {
      FileEntry fentry{file_path};
      size_t i = 0;
      while (fentry.has_chunk()) {
        std::tuple<std::shared_ptr<char[]>, size_t> chunk = fentry.next_chunk();
        json jentry = fentry.get_json_representation();
        rest_client->post_chunk(chunk, jentry);
        fentry.clear_chunks();
        i++;
      }
    }
  }
}
void SyncSubscriber::on_new_folder(const Message &message) {
  DurationLogger logger{"NEW_FOLDER"};
  json mex;
  std::shared_ptr<Broker> broker = Broker::getInstance();
  json content = message.get_content();
  for (auto &p :
       std::filesystem::recursive_directory_iterator(content["path"])) {
    std::string new_path = p.path().string();
    if (std::filesystem::is_regular_file(new_path)) {
      mex["path"] = new_path;
      on_new_file(Message{TOPIC::NEW_FILE, mex});
    }
  }
}
void SyncSubscriber::on_file_renamed(const Message &message) {
  // todo: inviare richiesta al server
  DurationLogger logger{"FILE_RENAMED"};
}

void SyncSubscriber::on_file_deleted(const Message &message) {
  DurationLogger logger{"FILE_DELETED"};
  json content = message.get_content();
  std::string path = content["path"];
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  rest_client->delete_file(path);
  compute_new_size();
}

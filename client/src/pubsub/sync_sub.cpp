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
  std::string file_path = message.get_content()["name"];
  if (std::filesystem::exists(file_path) &&
      !std::filesystem::exists(file_path)) {
    std::shared_ptr<Broker> broker = Broker::getInstance();
    json file_entry;
    file_entry["path"] = file_entry;
    file_entry["last_mod"] =
        std::filesystem::last_write_time(file_path).time_since_epoch().count();
    uintmax_t size = std::filesystem::file_size(file_path);
    file_entry["size"] = size;
    size_t n_chunks = ceil(size / CHUNK_SIZE);
    std::ifstream istream{file_path, std::ios::binary};
    Chunk c{std::move(istream)};
    for (size_t i = 0; i < n_chunks; i++) {
      size_t to_read = size > CHUNK_SIZE ? CHUNK_SIZE : size;
      c.read(i, to_read);
      size -= to_read;
      file_entry["chunks"].push_back(c.get_json_representation());
      broker->publish(TOPIC::ADD_CHUNK, Message{file_entry});
      file_entry["chunks"].clear();
      // todo: prepare http request and push to sync worker
    }
  }
}
void SyncSubscriber::on_new_folder(const Message &message) {
  std::clog << "New folder\n";
  json mex;
  std::shared_ptr<Broker> broker = Broker::getInstance();
  json content = message.get_content();
  for (auto &p :
       std::filesystem::recursive_directory_iterator(content["path"])) {
    std::string new_path = p.path().string();
    if (std::filesystem::is_regular_file(new_path)) {
      mex["path"] = new_path;
      broker->publish(TOPIC::NEW_FILE, Message{mex});
    }
  }
}
void SyncSubscriber::on_file_renamed(const Message &message) {
  std::clog << "New file_renamed\n";
  //todo: inviare richiesta al server
}
void SyncSubscriber::on_file_modified(const Message &message) {
  std::clog << "New file_modified\n";
}
void SyncSubscriber::on_file_deleted(const Message &message) {
  std::clog << "New file_deleted\n";
  // todo: invia al server
}
void SyncSubscriber::on_bulk_delete(const Message &message) {
  std::clog << "bulk_delete\n";
}

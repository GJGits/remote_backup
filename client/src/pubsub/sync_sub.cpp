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
  json content = message.get_content();
  std::string file_path = content["path"];
  std::clog << "fpath: " << file_path << "\n";
  if (std::filesystem::exists(file_path) &&
      !std::filesystem::is_empty(file_path)) {
    std::shared_ptr<Broker> broker = Broker::getInstance();
    FileEntry fentry{file_path};
    size_t n_chunks = fentry.get_nchunks();
    uintmax_t size = fentry.get_size();
    std::ifstream istream{file_path, std::ios::binary};
    Chunk c{std::move(istream)};
    for (size_t i = 0; i < n_chunks; i++) {
      size_t to_read = size > CHUNK_SIZE ? CHUNK_SIZE : size;
      c.read(i, to_read);
      size -= to_read;
      json chk = c.get_json_representation();
      fentry.add_chunk(chk);
      broker->publish(TOPIC::ADD_CHUNK,
                      Message{fentry.get_json_representation()});
      fentry.clear_chunks();
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
      on_new_file(mex);
    }
  }
}
void SyncSubscriber::on_file_renamed(const Message &message) {
  std::clog << "New file_renamed\n";
  // todo: inviare richiesta al server
}
void SyncSubscriber::on_file_modified(const Message &message) {
  std::clog << "New file_modified\n";
  std::shared_ptr<SyncStructure> structure = SyncStructure::getInstance();
  std::shared_ptr<Broker> broker = Broker::getInstance();
  json content = message.get_content();
  std::vector<std::string> hashes =
      structure->get_entry_hashes(content["path"]);
  FileEntry fentry{content["path"]};
  size_t n_chunks = fentry.get_nchunks();
  uintmax_t size = fentry.get_size();
  std::ifstream istream{content["path"], std::ios::binary};
  Chunk c{std::move(istream)};
  for (size_t i = 0; i < n_chunks; i++) {
    size_t to_read = size > CHUNK_SIZE ? CHUNK_SIZE : size;
    c.read(i, to_read);
    size -= to_read;
    json chk = c.get_json_representation();
    fentry.add_chunk(chk);
    // 1 chunk non presente su struct (modifica con append a destra) -> aggiungo
    // chunk
    if (i > hashes.size()) {
      broker->publish(TOPIC::ADD_CHUNK,
                      Message{fentry.get_json_representation()});
      // prepare http request
    }
    // 2 chunk presente, ma hash diverso (modifica interna) -> replace chunk
    if (i <= hashes.size() && hashes[i].compare(chk["hash"]) != 0) {
      broker->publish(TOPIC::UPDATE_CHUNK,
                      Message{fentry.get_json_representation()});
      // prepare http request
    }
    fentry.clear_chunks();
  }
  // 3 chunk presente su struct, ma non su filesystem (delete parziale) ->
  // delete chunk
  for (size_t j = n_chunks; j < hashes.size(); j++) {
    json en;
    en["path"] = content["path"];
    en["id"] = j;
    broker->publish(TOPIC::DELETE_CHUNK, Message{en});
    // prepare http request
  }
}
void SyncSubscriber::on_file_deleted(const Message &message) {
  std::clog << "New file_deleted\n";
  // todo: invia al server
}
void SyncSubscriber::on_bulk_delete(const Message &message) {
  std::clog << "bulk_delete\n";
}

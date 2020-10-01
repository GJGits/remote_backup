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
  remote_check();
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
  std::shared_ptr<Broker> broker = Broker::getInstance();
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
        broker->publish(Message{TOPIC::ADD_CHUNK, jentry});
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
  std::shared_ptr<Broker> broker = Broker::getInstance();
  json content = message.get_content();
  std::string path = content["path"];
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  rest_client->delete_file(path);
  broker->publish(Message{TOPIC::REMOVE_ENTRY, content});
  compute_new_size();
}

void SyncSubscriber::remote_check() {
  DurationLogger logger{"PROCESS_LIST"};
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  std::shared_ptr<Broker> broker = Broker::getInstance();
  int current_page = 0;
  int last_page = 1;
  while (current_page < last_page) {
    json list = rest_client->get_status_list(current_page++);
    std::clog << "status list:\n" << list.dump() << "\n";
    current_page = list["current_page"];
    last_page = list["last_page"].get<int>();
    // todo: processa messaggi ed eventualmente invia conflitti all'utente.
    for (size_t i = 0; i < list["entries"].size(); i++) {

      /*
      std::string file_path =
          macaron::Base64::Decode(list["entries"][i]["path"]);
      int nchunks = list["entries"][i]["num_chunks"].get<int>();
      int last_mod_remote = list["entries"][i]["last_mod"].get<int>();
      if (!std::filesystem::exists(file_path)) {
        // server ha file che io non ho
        std::filesystem::create_directories(
            std::filesystem::path{file_path}.parent_path());
        std::ofstream out{file_path, std::ios::binary};
        for (int j = 0; j < nchunks; j++) {
          json get_chunk{{"path", list["entries"][i]["path"]}, {"id", j}};
          std::vector<char> chunk = rest_client->get_chunk(get_chunk);
          json jentry{{"nchunks", nchunks},
                      {"size", 0},
                      {"path", file_path},
                      {"last_mod", last_mod_remote}};
          json jchunk{{"hash", Sha256::getSha256(chunk)}, {"id", j}};
          out.write(chunk.data(), chunk.size());
          jentry["chunks"].push_back(jchunk);
          broker->publish(Message{TOPIC::ADD_CHUNK, jentry});
        }
        out.close();
      } else {
        int last_mod_local = std::filesystem::last_write_time(file_path)
                                 .time_since_epoch()
                                 .count() /
                             1000000000;
        if (last_mod_remote > last_mod_local) {
          // server ha versione aggiornata
          std::ofstream out{file_path, std::ios::binary};
          for (int j = 0; j < nchunks; j++) {
            json get_chunk{{"path", list["entries"][i]["path"]}, {"id", j}};
            std::vector<char> chunk = rest_client->get_chunk(get_chunk);
            json jentry{{"nchunks", nchunks},
                        {"size", 0},
                        {"path", file_path},
                        {"last_mod", last_mod_remote}};
            json jchunk{{"hash", Sha256::getSha256(chunk)}, {"id", j}};
            out.write(chunk.data(), chunk.size());
            jentry["chunks"].push_back(jchunk);
            broker->publish(Message{TOPIC::ADD_CHUNK, jentry});
          }
          out.close();
        }
      }
      */
    }
  }
}

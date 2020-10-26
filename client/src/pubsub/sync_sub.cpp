#include "../../include/pubsub/sync_sub.hpp"

void SyncSubscriber::init_sub_list() {
  std::shared_ptr<Broker> broker = Broker::getInstance();
  broker->subscribe(
      TOPIC::LOGGED_IN,
      std::bind(&SyncSubscriber::start, instance.get(), std::placeholders::_1));
  broker->subscribe(
      TOPIC::LOGGED_OUT,
      std::bind(&SyncSubscriber::stop, instance.get(), std::placeholders::_1));
  broker->subscribe(
      TOPIC::CLOSE,
      std::bind(&SyncSubscriber::stop, instance.get(), std::placeholders::_1));
  broker->subscribe(TOPIC::NEW_FILE,
                    std::bind(&SyncSubscriber::on_new_file, instance.get(),
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

void SyncSubscriber::start(const Message &message) {
  std::clog << "sync module started...\n";
  running = true;
  init_workers();
  std::unordered_map<std::string, json> changes;
  // 1. collect transfers
  collect_unfinished_transfers(changes);
  // 2. collect local changes
  collect_local_changes(changes);
  // 3. collect remote changes
  // collect_remote_changes(changes);
  // 4. run corrections
  run_corrections(changes);
}

void SyncSubscriber::stop(const Message &message) {
  std::clog << "sync module closed...\n";
  running = false;
  cv.notify_all();
}

void SyncSubscriber::push(const json &task) {
  std::unique_lock lk{m};
  down_tasks.push(task);
  cv.notify_one();
}

void SyncSubscriber::on_new_file(const Message &message) {
  DurationLogger logger{"NEW_FILE"};
  std::shared_ptr<Broker> broker = Broker::getInstance();
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  json content = message.get_content();
  std::string file_path = content["path"];
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

void SyncSubscriber::on_file_renamed(const Message &message) {
  // todo: inviare richiesta al server
  DurationLogger logger{"FILE_RENAMED"};
  std::shared_ptr<RestClient> rest = RestClient::getInstance();
  std::shared_ptr<Broker> broker = Broker::getInstance();
  json content = message.get_content();
  rest->rename_file(content["old_path"].get<std::string>(),
  content["new_path"].get<std::string>());
  broker->publish(Message{TOPIC::ENTRY_RENAMED, content});
}

void SyncSubscriber::on_file_deleted(const Message &message) {
  DurationLogger logger{"FILE_DELETED"};
  std::shared_ptr<Broker> broker = Broker::getInstance();
  json content = message.get_content();
  std::string path = content["path"];
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  rest_client->delete_file(path);
  broker->publish(Message{TOPIC::REMOVE_ENTRY, content});
}

void SyncSubscriber::collect_unfinished_transfers(
    std::unordered_map<std::string, json> &changes) {
  std::shared_ptr<SyncStructure> syn = SyncStructure::getInstance();
  std::vector<json> entries = syn->get_incomplete_entries();
  for (size_t i = 0; i < entries.size(); i++) {
    json change = {{"transfers", entries[i]}};
    changes[entries[i]["path"].get<std::string>()] = change;
  }
}

void SyncSubscriber::collect_local_changes(
    std::unordered_map<std::string, json> &changes) {
  std::shared_ptr<SyncStructure> syn = SyncStructure::getInstance();
  int last_check = syn->get_last_check();
  // colleziono nuovi file aggiunti o modificati offline
  for (auto &p : std::filesystem::recursive_directory_iterator("./sync")) {
    if (p.is_regular_file()) {
      std::string file_path = p.path().string();
      struct stat sb;
      stat(file_path.c_str(), &sb);
      int last_change = sb.st_ctime;
      if (last_change > last_check) {
        json change = {{"local",
                        {{"path", file_path},
                         {"last_local_change", last_change},
                         {"command", "update"}}}};
        changes[file_path] = change;
      }
    }
  }
  // colleziono file eliminati offline
  for (const auto &path : syn->get_paths()) {
    if (!std::filesystem::exists(path)) {
      json change = {{"local",
                      {{"path", path},
                       {"last_local_change", syn->get_last_local_change(path)},
                       {"command", "delete"}}}};
      changes[path] = change;
    }
  }
}

void SyncSubscriber::collect_remote_changes(
    std::unordered_map<std::string, json> &changes) {
  DurationLogger logger{"PROCESS_LIST"};
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  std::shared_ptr<SyncStructure> sync_structure = SyncStructure::getInstance();
  int current_page = 0;
  int last_page = 1;
  while (current_page < last_page) {
    json list = rest_client->get_status_list(current_page++);
    std::clog << "list dump: " << list.dump() << "\n";
    current_page = list["current_page"].get<int>();
    last_page = list["last_page"].get<int>();
    for (size_t i = 0; i < list["entries"].size(); i++) {
      std::string file_path =
          macaron::Base64::Decode(list["entries"][i]["path"]);
      json change = {{"remote", list["entries"][i]}};
      changes[file_path] = change;
    }
  }
}

void SyncSubscriber::run_corrections(
    std::unordered_map<std::string, json> &changes) {
  for (auto const &[key, val] : changes) {
    int transfer_timestamp = 0;
    int local_change = 0;
    int remote_change = 0;
    if (val.find("transfers") != val.end()) {
      transfer_timestamp = val["transfers"]["last_local_change"].get<int>();
    }
    if (val.find("local") != val.end()) {
      local_change = val["local"]["last_local_change"].get<int>();
    }
    if (val.find("remote") != val.end()) {
      remote_change = val["remote"]["last_remote_change"].get<int>();
    }
    if (transfer_timestamp > local_change &&
        transfer_timestamp > remote_change) {
      // todo: completa trasferimenti
    }
    if (local_change > remote_change && local_change > transfer_timestamp) {
      if (val["local"]["command"].get<std::string>().compare("update") == 0) {
        json mex = val["local"];
        on_new_file(Message(TOPIC::NEW_FILE, mex));
      } else {
        json mex = val["local"];
        on_file_deleted(Message(TOPIC::FILE_DELETED, mex));
      }
    }
    if (remote_change > local_change && remote_change > transfer_timestamp) {
      json task = val["remote"];
      push(task);
    }
  }
}

void SyncSubscriber::init_workers() {
  for (size_t i = 0; i < 2; i++) {
    down_workers.emplace_back([&]() {
      std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
      std::shared_ptr<Broker> broker = Broker::getInstance();
      while (running) {
        json task;
        // sezione critica
        {
          std::unique_lock lk{m};
          if (!down_tasks.empty()) {
            task = down_tasks.front();
            down_tasks.pop();
          } else {
            restore_files();
            cv.wait(lk, [&]() { return !down_tasks.empty() || !running; });
            continue;
          }
        }
        // sezione non critica
        if (std::string{task["command"]}.compare("updated") == 0) {
          for (int j = 0; j < task["num_chunks"].get<int>(); j++) {
            json chunk_info = {{"path", task["path"]}, {"id", j}};
            std::vector<char> chunk = rest_client->get_chunk(chunk_info);
            std::string folder = "./sync/.tmp/" + std::string{task["path"]};
            std::string file_path = macaron::Base64::Decode(task["path"]);
            std::string chunk_path = folder + "/" + std::string{task["path"]} +
                                     "_" + std::to_string(j) + ".chk";
            std::filesystem::create_directories(std::filesystem::path{folder});
            std::ofstream out{chunk_path, std::ios::binary};
            out.write(chunk.data(), chunk.size());
            out.close();
            std::string chunk_hash = Sha256::getSha256(chunk);
            json entry = {{"path", file_path},
                          {"nchunks", task["num_chunks"]},
                          {"direction", "down"}};
            entry["chunks"].push_back({{"id", task["id"].get<int>()}});
            broker->publish(Message{TOPIC::ADD_CHUNK, entry});
          }
        } else {
          std::string file_path = macaron::Base64::Decode(task["path"]);
          std::string new_path =
              std::string{"./sync/.bin/"} + std::string{task["path"]};
          std::filesystem::rename(file_path, new_path);
          std::remove(new_path.c_str());
          broker->publish(Message{TOPIC::REMOVE_ENTRY, task});
        }
      }
    });
  }
}

void SyncSubscriber::restore_files() {
  for (auto &p : std::filesystem::directory_iterator("./sync/.tmp")) {
    if (p.is_directory()) {
      std::vector<std::string> chunks{};
      for (const auto &entry : std::filesystem::directory_iterator(p.path())) {
        if (entry.is_regular_file()) {
          chunks.push_back(entry.path().string());
        }
      }
      std::sort(
          chunks.begin(), chunks.end(), [&](std::string &s1, std::string &s2) {
            std::vector<std::string> tok11 = Utility::split(s1, '_');
            std::vector<std::string> tok12 = Utility::split(s2, '_');
            std::vector<std::string> tok21 = Utility::split(tok11[1], '.');
            std::vector<std::string> tok22 = Utility::split(tok21[1], '.');
            return std::stoi(tok21[0]) < std::stoi(tok22[0]);
          });
      std::string new_path =
          macaron::Base64::Decode(p.path().filename().string());
      std::clog << "new path: " << new_path << "\n";
      std::string temp_path = p.path().string() + ".out";
      std::ofstream out{temp_path, std::ios::binary};
      for (size_t i = 0; i < chunks.size(); i++) {
        std::ifstream in{chunks[i], std::ios::binary};
        int fsize = std::filesystem::file_size(chunks[i]);
        char buff[fsize];
        memset(buff, '\0', fsize);
        in.read(buff, fsize);
        out.write(buff, fsize);
      }
      std::filesystem::rename(temp_path, new_path);
    }
  }
}

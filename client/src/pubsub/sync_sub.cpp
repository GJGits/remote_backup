#include "../../include/pubsub/sync_sub.hpp"

std::shared_ptr<SyncSubscriber> SyncSubscriber::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<SyncSubscriber>(new SyncSubscriber{});
  }
  return instance;
}

void SyncSubscriber::init_sub_list() {
  std::shared_ptr<Broker> broker = Broker::getInstance();
  broker->subscribe(
      TOPIC::LOGGED_IN,
      std::bind(&SyncSubscriber::start, instance.get(), std::placeholders::_1));
  broker->subscribe(
      TOPIC::LOGGED_OUT,
      std::bind(&SyncSubscriber::stop, instance.get(), std::placeholders::_1));
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

void SyncSubscriber::start(const Message &message) {
  running = true;
  compute_new_size();
  remote_check();
  init_workers();
}

void SyncSubscriber::stop(const Message &message) { running = false; }

void SyncSubscriber::push(const json &task) {
  std::unique_lock lk{m};
  down_tasks.push(task);
  cv.notify_one();
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
  std::shared_ptr<SyncStructure> sync_structure = SyncStructure::getInstance();
  int current_page = 0;
  int last_page = 1;
  while (current_page < last_page) {
    json list = rest_client->get_status_list(current_page++);
    std::clog << "list dump: " << list.dump() << "\n";
    current_page = list["current_page"].get<int>();
    last_page = list["last_page"].get<int>();
    for (size_t i = 0; i < list["entries"].size(); i++) {
      json task = list["entries"][i];
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
                          {"last_mod", task["last_mod"]},
                          {"size", chunk.size()},
                          {"nchunks", task["num_chunks"]}};
            entry["chunks"].push_back(
                {{"id", task["id"].get<int>()}, {"hash", chunk_hash}});
            broker->publish(Message{TOPIC::ADD_CHUNK, entry});
          }
        } else {
          std::string file_path = macaron::Base64::Decode(task["path"]);
          std::string new_path =
              std::string{"./sync/.bin/"} + std::string{task["path"]};
          std::filesystem::rename(file_path, new_path);
          std::remove(new_path.c_str());
          broker->publish(Message{TOPIC::REMOVE_ENTRY, task});
          compute_new_size();
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

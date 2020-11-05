#include "../../include/pubsub/sync_sub.hpp"

SyncSubscriber::SyncSubscriber() : running{false} {
  std::clog << "Sync module init...\n";
}

SyncSubscriber::~SyncSubscriber() {
  running = false;
  for (size_t i = 0; i < down_workers.size(); i++) {
    down_workers[i].join();
  }
  std::clog << "Sync module destroy...\n";
}

void SyncSubscriber::init_sub_list() {
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
  init_workers();
}

void SyncSubscriber::stop() {
  std::clog << "sync module stop...\n";
  running = false;
  cv.notify_all();
}

void SyncSubscriber::push(const std::shared_ptr<FileEntry> &task) {
  std::unique_lock lk{m};
  down_tasks.push(task);
  cv.notify_one();
}

void SyncSubscriber::on_new_file(const Message &message) {
  DurationLogger logger{"NEW_FILE"};
  std::shared_ptr<FileEntry> fentry = message.get_content();
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  if (fentry->get_producer() == entry_producer::local) {
    broker->publish(Message{TOPIC::ADD_ENTRY, fentry});
    while (fentry->has_chunk()) {
      std::tuple<std::shared_ptr<char[]>, size_t> chunk = fentry->next_chunk();
      rest_client->post_chunk(chunk, fentry->to_string());
      fentry->update_read_count();
    }
    fentry->set_status(entry_status::synced);
  }
  if (fentry->get_producer() == entry_producer::server) {
    push(fentry);
  }
}

void SyncSubscriber::on_file_deleted(const Message &message) {
  DurationLogger logger{"FILE_DELETED"};
  std::shared_ptr<FileEntry> fentry = message.get_content();
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  if (fentry->get_producer() == entry_producer::local) {
    rest_client->delete_file(fentry->get_path());
    broker->publish(Message{TOPIC::REMOVE_ENTRY, fentry});
  }
  if (fentry->get_producer() == entry_producer::server) {
    std::filesystem::rename(fentry->get_path(), "./sync/.bin/a");
    std::remove("./sync/.bin/a");
    broker->publish(Message{TOPIC::REMOVE_ENTRY, fentry});
  }
}

void SyncSubscriber::init_workers() {
  for (size_t i = 0; i < 2; i++) {
    down_workers.emplace_back([&]() {
      while (running) {
        std::shared_ptr<FileEntry> task;
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
        for (size_t i = 0; i < task->get_nchunks(); i++) {
          task->retrieve_chunk();
          task->update_read_count();
        }
        task->set_status(entry_status::synced);
      }
    });
  }
}

void SyncSubscriber::restore_files() {

  if (!std::filesystem::exists("./sync"))
    throw SyncNotValid();

  if (!std::filesystem::exists("./sync/.tmp")) {
    std::clog << "tmp oddio\n";
    throw TmpNotValid();
  }

  if (!std::filesystem::exists("./sync/.bin"))
    throw BinNotValid();

  for (auto &p : std::filesystem::directory_iterator("./sync/.tmp")) {
    if (p.is_directory()) {
      std::vector<std::string> chunks{};
      for (const auto &entry : std::filesystem::directory_iterator(p.path())) {
        if (entry.is_regular_file()) {
          chunks.push_back(entry.path().string());
        }
      }

      std::sort(
          chunks.begin(), chunks.end(),
          [&](std::string &s1, std::string &s2) { return s1.compare(s2); });

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
      std::filesystem::remove_all(p);
    }
  }
}

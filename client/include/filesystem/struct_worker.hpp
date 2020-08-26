#pragma once

#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../common/json.hpp"
#include "handle_watcher.hpp"
#include "sync_structure.hpp"

class StructWorker {
private:
  bool finish;
  std::thread worker;
  static inline StructWorker *instance = nullptr;
  std::mutex m;
  std::condition_variable cv;

public:
  ~StructWorker() {
    finish = true;
    worker.join();
  }

  static StructWorker *getInstance() {
    if (instance == nullptr) {
      instance = new StructWorker{};
    }
    return instance;
  }

  void run_worker() {
    finish = false;
    expand("./sync");
    prune();
    worker = std::move(std::thread{[&]() {
      HandleWatcher *dispatcher = HandleWatcher::getInstance();
      SyncStructure *sync = SyncStructure::getInstance();
      while (!finish) {

        std::shared_ptr<Message> raw_message = dispatcher->pop_message(0);

        if (raw_message->getType() != -1) {
          std::shared_ptr<StructMessage> message =
              std::dynamic_pointer_cast<StructMessage>(raw_message);
          switch (message->getCode()) {

          case MESSAGE_CODE::ADD_CHUNK:
            add_chunk(message->getArgument());
            break;

          case MESSAGE_CODE::EXPAND:
            expand(message->getArgument()["name"]);
            break;

          case MESSAGE_CODE::INCREASE_COUNTER:
            sync->increase_counter();
            break;

          case MESSAGE_CODE::PRUNING:
            prune();
            break;

          case MESSAGE_CODE::REMOVE:
            delete_entry(message->getArgument());
            break;

          case MESSAGE_CODE::UPDATE_PATH:
            rename(message->getArgument());
            break;

          default:
            break;
          }
        }
      }
    }});
  }

  void expand(const std::string &path) {
    HandleWatcher *dispatcher = HandleWatcher::getInstance();
    SyncStructure *sync_structure = SyncStructure::getInstance();
    std::vector<std::string> added;
    if (std::filesystem::is_directory(path) &&
        !std::filesystem::is_empty(path)) {
      for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
        std::string sub_path = p.path().string();
        struct stat fileInfo;
        stat(path.c_str(), &fileInfo);
        int last_mod = (int)fileInfo.st_mtim.tv_sec;
        if (std::filesystem::is_regular_file(sub_path) &&
            !sync_structure->has_entry(sub_path) &&
            sync_structure->is_updated(sub_path, last_mod) &&
            !std::filesystem::is_empty(sub_path)) {
          added.push_back(sub_path);
        }
      }
    }
    if (!added.empty()) {
      dispatcher->push_message(std::make_shared<SyncMessage>(
          SyncMessage{MESSAGE_CODE::BULK_UPLOAD, added}));
    }
  }

  void add_chunk(const json &entry) {
    DurationLogger duration{"ADD_CHUNK"};
    if (std::filesystem::exists(entry["path"]) &&
        !std::filesystem::is_empty(entry["path"])) {
      SyncStructure *sync = SyncStructure::getInstance();
      sync->add_entry(entry);
      sync->write_structure();
    }
  }

  void delete_entry(const json &entry) {
    std::clog << " Evento: InDelete, path :" << entry["name"] << "\n";
    SyncStructure *sync = SyncStructure::getInstance();
    sync->remove_entry(entry["name"]);
  }

  void prune() {
    std::clog << " Evento: Pruning\n";
    HandleWatcher *dispatcher = HandleWatcher::getInstance();
    SyncStructure *sync = SyncStructure::getInstance();
    if (sync->get_count() > 0) {
      std::vector<std::string> deleted = sync->prune();
      sync->reset_counter();
      if (!deleted.empty()) {
        dispatcher->push_message(std::make_shared<SyncMessage>(
            SyncMessage{MESSAGE_CODE::BULK_DELETE, deleted}));
      }
    }
    sync->write_structure();
  }

  /*
  void handle_InModify(const std::string &path) {
    std::clog << " Evento: InModify, path :" << path << "\n";
    SyncStructure *sync = SyncStructure::getInstance();
    FileEntry entry = std::move(sync->add_entry(path));
  }
*/

  void rename(const json &entry) {
    std::clog << " Evento: InRename, old_path :" << entry["old"]
              << ", new_path: " << entry["new"] << "\n";
    SyncStructure *sync = SyncStructure::getInstance();
    sync->rename_entry(entry["old"], entry["new"]);
  }
};
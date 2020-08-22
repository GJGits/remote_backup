#pragma once

#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <string>
#include <thread>

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
    worker = std::move(std::thread{[&]() {
      HandleWatcher *dispatcher = HandleWatcher::getInstance();
      while (!finish) {

        std::shared_ptr<Message> raw_message = dispatcher->pop_message(0);

        if (raw_message->getType() != -1) {
          std::shared_ptr<StructMessage> message =
              std::dynamic_pointer_cast<StructMessage>(raw_message);
          switch (message->getCode()) {

          case MESSAGE_CODE::ADD_CHUNK:
            add_chunk(message->getArgument());
            break;

          /*
          case MESSAGE_CODE::PRUNING:
            handle_prune();
          break;
          */
          default:
            break;
          }
        }
      }
    }});
  }

  void add_chunk(const json &entry) {
    std::clog << "add_chunk\n";
    std::clog << "entry_path: " << entry["path"] << "\n";
    if (std::filesystem::exists(entry["path"]) &&
        !std::filesystem::is_empty(entry["path"])) {
      SyncStructure *sync = SyncStructure::getInstance();
      sync->add_entry(entry);
      sync->write_structure();
    }
  }

  void handle_InDelete(const std::string &path) {
    std::clog << " Evento: InDelete, path :" << path << "\n";
    SyncStructure *sync = SyncStructure::getInstance();
    sync->remove_entry(path);
  }

  void handle_prune() {
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

  void handle_InRename(const std::string &old_path,
                       const std::string &new_path) {
    std::clog << " Evento: InRename, old_path :" << old_path
              << ", new_path: " << new_path << "\n";
    SyncStructure *sync = SyncStructure::getInstance();
    sync->rename_entry(old_path, new_path);
  }
  */
};
#pragma once

#include <filesystem>
#include <string>
#include <thread>

#include "handle_watcher.hpp"
#include "sync_structure.hpp"

class StructWorker {
private:
  bool finish;
  std::thread worker;
  static inline StructWorker *instance = nullptr;

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
    worker = std::move(std::thread{[&]() {
      HandleWatcher *event_handler = HandleWatcher::getInstance();
      SyncStructure *sync_structure = SyncStructure::getInstance();
      while (!finish) {
        try {
          Message mex = event_handler->pop_message(true);
          switch (mex.getCode()) {

          case MESSAGE_CODE::INSERT:
            handle_InCloseWrite(mex.getArguments()[0]);
            break;

          case MESSAGE_CODE::BULK_INSERT:
            handle_expand(mex.getArguments()[0]);
            break;

          case MESSAGE_CODE::MOVED:
            sync_structure->increase_counter();
            break;

          case MESSAGE_CODE::REMOVE:
            handle_InDelete(mex.getArguments()[0]);
            break;

          case MESSAGE_CODE::RENAME:
            if (mex.getArguments()[0].empty()) {
              handle_InCloseWrite(mex.getArguments()[1]);
            } else {
              handle_InRename(mex.getArguments()[0], mex.getArguments()[1]);
              sync_structure->decrease_counter();
            }

            break;

          case MESSAGE_CODE::PRUNING:
            handle_prune();
            break;

          default:
            break;
          }

        } catch (std::filesystem::__cxx11::filesystem_error &e) {
          std::clog << e.what() << "\n";
        } catch (...) {
          std::clog << "Errore sconosciuto\n";
        }
      }
    }});
  }

  void handle_InCloseWrite(const std::string &path) {
    std::clog << " Evento: InCloseWrite , path : " << path << "\n";

    if (std::filesystem::exists(path) && !std::filesystem::is_empty(path)) {
      SyncStructure *sync = SyncStructure::getInstance();
      FileEntry entry = std::move(sync->add_entry(path));
    }
  }

  void handle_expand(const std::string &path) {
    std::clog << " Evento: expand , path : " << path << "\n";
    if (std::filesystem::is_directory(path) &&
        !std::filesystem::is_empty(path)) {
      SyncStructure *sync = SyncStructure::getInstance();
      for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
        // todo: check anche se file_entry modificata
        if (std::filesystem::is_regular_file(p.path().string()) &&
            !sync->has_entry(p.path().string()) &&
            !std::filesystem::is_empty(p.path().string())) {
          FileEntry entry = std::move(sync->add_entry(p.path().string()));
        }
      }
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
    SyncStructure *sync = SyncStructure::getInstance();
    if (sync->get_count() > 0) {
      std::vector<std::string> deleted = sync->prune();
      sync->reset_counter();
      if (!deleted.empty()) {
        // todo invia al server
      }
    }
    sync->write_structure();
  }

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
};
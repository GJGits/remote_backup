//
// Created by alessandra on 30/07/20.
//

#include "../../include/filesystem/handle_watcher.hpp"
#include "../../include/filesystem/linux_watcher.hpp"
#include "../../include/filesystem/sync_structure.hpp"

HandleWatcher *HandleWatcher::getInstance() {
  if (instance == nullptr) {
    instance = new HandleWatcher();
    instance->run_workers();
  }
  return instance;
}

HandleWatcher::~HandleWatcher() {
  finish = true;
  for (size_t i = 0; i < workers.size(); i++) {
    workers[i].join();
  }
}

void HandleWatcher::run_workers() {
  for (size_t i = 0; i < 8; i++) {
    workers.emplace_back([&]() {
      while (!finish) {
        std::unique_lock lk{m};
        if (!events.empty()) {
          Event event = events.front();
          events.pop();
          lk.unlock();
          switch (event.getType()) {
          case EVENT_TYPE::CREATE:
            handle_InCloseWrite(event.getArgument1());
            break;
          case EVENT_TYPE::DELETE:
            handle_InDelete(event.getArgument1());
            break;
          case EVENT_TYPE::EXPAND:
            handle_expand(event.getArgument1());
            break;
          case EVENT_TYPE::RENAME:
            handle_InRename(event.getArgument1(), event.getArgument2());
            break;
          case EVENT_TYPE::PRUNING:
            handle_prune();
            break;
          default:
            break;
          }
        } else
          cv.wait(lk, [&]() { return !events.empty() || finish; });
      }
    });
  }
}

void HandleWatcher::push_event(const Event &event) {
  std::unique_lock lk{m};
  events.push(event);
  cv.notify_all();
}

void HandleWatcher::handle_InCloseWrite(const std::string &path) {
  std::clog << " Evento: InCloseWrite , path : " << path << "\n";

  if (std::filesystem::exists(path)) {
    SyncStructure *sync = SyncStructure::getInstance();
    sync->add_entry(path);
    sync->write_structure();
  }
}

void HandleWatcher::handle_expand(const std::string &path) {
  std::clog << " Evento: expand , path : " << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
    if (std::filesystem::is_regular_file(p.path().string())) {
      sync->add_entry(p.path().string());
    }
  }
  sync->write_structure();
}

void HandleWatcher::handle_InDelete(const std::string &path) {
  std::clog << " Evento: InDelete, path :" << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->remove_entry(path);
  sync->write_structure();
  // todo: send delete to server
}

void HandleWatcher::handle_prune() {
  std::clog << " Evento: Pruning\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->prune();
  sync->write_structure();
  // todo: send bulk_delete to server
}

void HandleWatcher::handle_InModify(const std::string &path) {
  std::clog << " Evento: InModify, path :" << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->add_entry(path);
  sync->write_structure();
}

void HandleWatcher::handle_InRename(const std::string &old_path, const std::string &new_path) {
  std::clog << " Evento: InRename, old_path :" << old_path << ", new_path: " << new_path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->rename_entry(old_path, new_path);
  sync->write_structure();
}

void HandleWatcher::handle_InMove(const std::string &path) {
  std::clog << " Evento: InMove, old_path =" << path << "\n";
}

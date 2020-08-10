//
// Created by alessandra on 30/07/20.
//

#include "../../include/filesystem/handle_watcher.hpp"
#include "../../include/filesystem/linux_watcher.hpp"
#include "../../include/filesystem/sync_structure.hpp"

void HandleWatcher::handle_InCloseWrite(std::string path) {
  std::clog << " Evento: InCloseWrite , path : " << path << "\n";
  if (std::filesystem::exists(path)) {
    SyncStructure *sync = SyncStructure::getInstance();
    sync->add_entry(path);
  }
}

void HandleWatcher::handle_expand(const std::string &path) {
  std::clog << " Evento: expand , path : " << path << "\n";
  for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
    if (std::filesystem::is_regular_file(p.path().string())) {
      handle_InCloseWrite(p.path().string());
    }
  }
}

void HandleWatcher::handle_InDelete(std::string path) {
  std::clog << " Evento: InDelete, path :" << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->remove_entry(path);
  // todo: send delete to server
}

void HandleWatcher::handle_prune() {
  std::clog << " Evento: Pruning\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->prune();
  // todo: send bulk_delete to server
}

void HandleWatcher::handle_InModify(std::string path) {
  std::clog << " Evento: InModify, path :" << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->add_entry(path);
}

void HandleWatcher::handle_InRename(std::string old_path,
                                    std::string new_path) {
  std::clog << " Evento: InRename, old_path =" << old_path
            << ", new_path = " << new_path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->add_entry(old_path, new_path);
}

void HandleWatcher::handle_InMove(std::string path) {
  std::clog << " Evento: InMove, old_path =" << path << "\n";
}

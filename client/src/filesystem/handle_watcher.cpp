//
// Created by alessandra on 30/07/20.
//

#include "../../include/filesystem/handle_watcher.hpp"
#include "../../include/filesystem/linux_watcher.hpp"
#include "../../include/filesystem/sync_structure.hpp"

void HandleWatcher::handle_InCreate(std::string path) {
  std::clog << " Evento: InCreate , cartella : " << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->add_entry(path);
}

void HandleWatcher::handle_InDelete(std::string path) {
  std::clog << " Evento: InDelete, cartella :" << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->remove_entry(path);
}

void HandleWatcher::handle_InModify(std::string path) {
  std::clog << " Evento: InModify, cartella :" << path << "\n";
}

void HandleWatcher::handle_InRename(std::string old_path,
                                    std::string new_path) {
  std::clog << " Evento: InRename, old_path =" << old_path
            << ", new_path = " << new_path << "\n";
}

void HandleWatcher::handle_InMove(std::string path) {
  std::clog << " Evento: InMove, old_path =" << path << "\n";
}

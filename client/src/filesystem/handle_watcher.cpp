//
// Created by alessandra on 30/07/20.
//

#include "../../include/filesystem/handle_watcher.hpp"

void Handle_watcher::handle_InCreate(std::string path) {
  std::clog << " Evento: InCreate , cartella : " << path << "\n";
  if (!std::filesystem::exists("../config/client-struct.json")) {
    std::ofstream file("../config/client-struct.json");
    file << std::string{"{\"hashed_status\":\"empty\",\n\"entries\":[]\n}"};
  }
  // todo: insert new entry
}

void Handle_watcher::handle_InDelete(std::string path) {
  std::clog << " Evento: InDelete, cartella :" << path << "\n";
  // todo: remove entry
}

void Handle_watcher::handle_InModify(std::string path) {
  std::clog << " Evento: InModify, cartella :" << path << "\n";
}

void Handle_watcher::handle_InRename(std::string old_path,
                                     std::string new_path) {
  std::clog << " Evento: InRename, old_path =" << old_path
            << ", new_path = " << new_path << "\n";
}
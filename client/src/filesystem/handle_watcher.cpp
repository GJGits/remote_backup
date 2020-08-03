//
// Created by alessandra on 30/07/20.
//

#include "../../include/filesystem/file_entry.hpp"
#include "../../include/filesystem/handle_watcher.hpp"
#include "../../include/filesystem/linux_watcher.hpp"

void HandleWatcher::handle_InCreate(std::string path) {
  std::clog << " Evento: InCreate , cartella : " << path << "\n";

  json structure;
  std::ifstream i("./config/client-struct.json");
  i >> structure;
  FileEntry fentry{path};
  json entry = {{"path", path},
                {"hash", fentry.getFileHash()},
                {"last_mod", 123456789},
                {"chunks", fentry.getChunks()},
                {"dim_last_chunk", fentry.getLastChunk()}};
  structure["entries"].push_back(entry);
  std::clog << structure << "\n";
  std::ofstream o("./config/client-struct.json");
  o << structure << "\n";
}

void HandleWatcher::handle_InDelete(std::string path) {
  std::clog << " Evento: InDelete, cartella :" << path << "\n";
  // todo: remove entry
}

void HandleWatcher::handle_InModify(std::string path) {
  std::clog << " Evento: InModify, cartella :" << path << "\n";
}

void HandleWatcher::handle_InRename(std::string old_path,
                                    std::string new_path) {
  std::clog << " Evento: InRename, old_path =" << old_path
            << ", new_path = " << new_path << "\n";
}
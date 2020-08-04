//
// Created by alessandra on 30/07/20.
//

#include "../../include/filesystem/handle_watcher.hpp"
#include "../../include/filesystem/file_entry.hpp"
#include "../../include/filesystem/linux_watcher.hpp"

json HandleWatcher::hash_struct(json &structure) {
  std::string entries_dump = structure["entries"].dump();
  std::vector<char> data(entries_dump.begin(), entries_dump.end());
  structure["hashed_status"] = structure["entries"].empty()
                                   ? std::string{"empty_hashed_status"}
                                   : Sha256::getSha256(data);
  return structure;
}

json HandleWatcher::read_structure() {
  json structure;
  std::ifstream i("./config/client-struct.json");
  i >> structure;
  return structure;
}
void HandleWatcher::write_structure(const json &structure) {
  std::ofstream o("./config/client-struct.json");
  o << structure << "\n";
}

void HandleWatcher::handle_InCreate(std::string path) {
  std::clog << " Evento: InCreate , cartella : " << path << "\n";

  json structure = read_structure();
  FileEntry fentry{path};
  json entry = {{"path", path},
                {"hash", fentry.getFileHash()},
                {"last_mod", fentry.getLastMod()},
                {"chunks", fentry.getChunks()},
                {"dim_last_chunk", fentry.getLastChunk()}};
  structure["entries"].push_back(entry);
  write_structure(hash_struct(structure));
}

void HandleWatcher::handle_InDelete(std::string path) {
  std::clog << " Evento: InDelete, cartella :" << path << "\n";
  json structure = read_structure();
  structure["entries"].erase(
      std::remove_if(structure["entries"].begin(), structure["entries"].end(),
                     [&](const json &x) {
                       return std::string{x["path"]}.compare(path) == 0;
                     }));
  write_structure(hash_struct(structure));
}

void HandleWatcher::handle_InModify(std::string path) {
  std::clog << " Evento: InModify, cartella :" << path << "\n";
}

void HandleWatcher::handle_InRename(std::string old_path,
                                    std::string new_path) {
  std::clog << " Evento: InRename, old_path =" << old_path
            << ", new_path = " << new_path << "\n";
}
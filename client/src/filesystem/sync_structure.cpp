#include "../../include/filesystem/sync_structure.hpp"

SyncStructure::SyncStructure() : last_check{0} {}

void SyncStructure::store() {
  std::ofstream o{"./config/client-struct.json"};
  json jstru = {{"entries", json::array()}, {"last_check", (int)std::time(nullptr)}};
  for (const auto &[path, fentry] : structure) {
    json entry = {{"path", path},
                  {"last_change", fentry->get_last_change()},
                  {"status", fentry->get_status()},
                  {"producer", fentry->get_producer()},
                  {"nchunks", fentry->get_nchunks()}};
    jstru["entries"].push_back(entry);
  }
  o << jstru << "\n";
}

void SyncStructure::restore() {
  std::ifstream i{"./config/client-struct.json"};
  json j;
  i >> j;
  last_check = j["last_check"].get<int>();
  for (size_t x = 0; x < j["entries"].size(); x++) {
    std::string path = j["entries"]["path"].get<std::string>();
    entry_producer producer =
        (entry_producer)j["entries"]["producer"].get<int>();
    size_t last_change = j["entries"]["last_change"].get<int>();
    entry_status status = (entry_status)j["entries"]["status"].get<int>();
    size_t nchunks = (size_t)j["entries"]["nchunks"].get<int>();
    std::shared_ptr<FileEntry> entry{
        new FileEntry{path, producer, nchunks, last_change, status}};
    add_entry(entry);
  }
}

void SyncStructure::update_from_fs() {
  for (const auto &p :
       std::filesystem::recursive_directory_iterator("./sync")) {
    if (p.is_regular_file()) {
      std::string path = p.path().string();
      entry_producer producer = entry_producer::folder;
      struct stat sb;
      stat(path.c_str(), &sb);
      size_t last_change = (size_t)sb.st_ctime;
      entry_status status = entry_status::new_;
      size_t nchunks =
          ceil((double)std::filesystem::file_size(path) / CHUNK_SIZE);
      std::shared_ptr<FileEntry> entry{
          new FileEntry{path, producer, nchunks, last_change, status}};
      if (last_change > last_check)
        add_entry(entry);
    }
  }
}

void SyncStructure::update_from_remote() {
  // aggiornamenti fake da server (risultato get_list)
  json list{{"entries", json::array()}};
  for (size_t y = 0; y < list["entries"].size(); y++) {
    std::string path = list["entries"][y]["path"];
    size_t last_change = list["entries"][y]["last_remote_change"].get<int>();
    entry_status status =
        list["entries"][y]["command"].get<std::string>().compare("updated") == 0
            ? entry_status::new_
            : entry_status::delete_;
    size_t nchunks = (size_t)list["entries"]["nchunks"].get<int>();
    std::shared_ptr<FileEntry> entry{new FileEntry{
        path, entry_producer::server, nchunks, last_change, status}};
    add_entry(entry);
  }
}

void SyncStructure::add_entry(const std::shared_ptr<FileEntry> &entry) {
  std::string path = entry->get_path();
  if (structure.find(path) == structure.end()) {
    structure[path] = entry;
  }
}

void SyncStructure::remove_entry(const std::shared_ptr<FileEntry> &entry) {
  structure.erase(entry->get_path());
}

std::vector<std::string> SyncStructure::get_paths() {
  std::vector<std::string> paths;
  for (const auto &[path, entry] : structure) {
    paths.push_back(path);
  }
  return paths;
}

std::vector<std::shared_ptr<FileEntry>> SyncStructure::get_entries() {
  std::vector<std::shared_ptr<FileEntry>> entries{};
  for (const auto &[key, entry] : structure) {
    entries.push_back(entry);
  }
  return entries;
}

size_t SyncStructure::get_last_check() const { return last_check; }
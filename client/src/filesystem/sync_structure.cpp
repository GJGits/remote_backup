#include "../../include/filesystem/sync_structure.hpp"

SyncStructure::SyncStructure() : server_ack{false}, last_check{0} {
  std::clog << "sync_struct init\n";
}

SyncStructure::~SyncStructure() { std::clog << "sync_struct destroy...\n"; }

void SyncStructure::store() {
  if (!structure.empty() && server_ack) {
    std::ofstream o{CLIENT_STRUCT};
    json jstru = {{"entries", json::array()},
                  {"last_check", (int)std::time(nullptr)}};
    for (const auto &[path, fentry] : structure) {
      json entry = fentry->to_json();
      jstru["entries"].push_back(entry);
    }
    o << jstru << "\n";
  }
}

void SyncStructure::restore() {
  std::ifstream i{CLIENT_STRUCT};
  i.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  json j;
  i >> j;
  last_check = j["last_check"].get<int>();
  for (size_t x = 0; x < j["entries"].size(); x++) {
    std::string path = j["entries"][x]["path"].get<std::string>();
    entry_producer producer =
        (entry_producer)j["entries"][x]["producer"].get<int>();
    size_t last_change = j["entries"][x]["last_change"].get<int>();

    entry_status status = entry_status::synced;

    if (std::filesystem::exists(path) ||
        (!std::filesystem::exists(path) &&
         (entry_status)j["entries"][x]["status"].get<int>() ==
             entry_status::new_)) {
      status = (entry_status)j["entries"][x]["status"].get<int>();
    } else if (!std::filesystem::exists(path) &&
               (entry_status)j["entries"][x]["status"].get<int>() ==
                   entry_status::synced) {
      status = entry_status::delete_;
    }

    size_t nchunks = (size_t)j["entries"][x]["nchunks"].get<int>();
    std::shared_ptr<FileEntry> entry{
        new FileEntry{path, producer, nchunks, last_change, status}};
    add_entry(entry);
  }
}

void SyncStructure::update_from_fs() {
  for (const auto &p :
       std::filesystem::recursive_directory_iterator(SYNC_ROOT)) {
    std::string path = p.path().string();
    if (!(path.rfind(TMP_PATH, 0) == 0) && !(path.rfind(BIN_PATH, 0) == 0) &&
        p.is_regular_file()) {
      std::shared_ptr<FileEntry> entry{
          new FileEntry{path, entry_producer::local, entry_status::new_}};
      // secondo caso possibile solo per rename di cartella che
      // va a modificare esclusivamente il change_time dell'inode
      // riferito alla cartella, gli inode dei file rimangono invariati.
      if ((entry->get_last_change() > last_check &&
           std::filesystem::file_size(path) > 0) ||
          (structure.find(path) == structure.end() &&
           std::filesystem::file_size(path) > 0))
        add_entry(entry);
    }
  }
}

void SyncStructure::update_from_remote() {
  int current_page = 0;
  int last_page = 1;
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  while (current_page < last_page) {
    json list = rest_client->get_status_list(current_page, last_check)["list"];
    last_page = list["last_page"].get<int>();
    for (size_t y = 0; y < list["entries"].size(); y++) {
      std::string path = macaron::Base64::Decode(
          list["entries"][y]["path"].get<std::string>());
      size_t last_change = list["entries"][y]["last_remote_change"].get<int>();
      entry_status status =
          list["entries"][y]["command"].get<std::string>().compare("updated") ==
                  0
              ? entry_status::new_
              : entry_status::delete_;
      size_t nchunks = (size_t)list["entries"][y]["num_chunks"].get<int>();
      std::shared_ptr<FileEntry> entry{new FileEntry{
          path, entry_producer::server, nchunks, last_change, status}};
      add_entry(entry);
    }
    current_page++;
  }
  server_ack = true;
}

void SyncStructure::add_entry(const std::shared_ptr<FileEntry> &entry) {
  std::string path = entry->get_path();
  if (structure.find(path) == structure.end()) {
    structure[path] = entry;
  } else {
    if (entry->get_last_change() > structure[path]->get_last_change())
      structure[path] = entry;
  }
}

void SyncStructure::remove_entry(const std::shared_ptr<FileEntry> &entry) {
  structure.erase(entry->get_path());
}

std::optional<std::shared_ptr<FileEntry>>
SyncStructure::get_entry(const std::string &path) {
  return structure.find(path) != structure.end()
             ? std::optional<std::shared_ptr<FileEntry>>{structure[path]}
             : std::nullopt;
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

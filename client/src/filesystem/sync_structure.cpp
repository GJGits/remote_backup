#include "../../include/filesystem/sync_structure.hpp"

/* PRIVATE SECTION */

void SyncStructure::create_structure() {
  if (!std::filesystem::exists("./config/client-struct.json") ||
      std::filesystem::is_empty("./config/client-struct.json")) {
    json temp;
    temp["hashed_status"] = "empty_hashed_status";
    temp["entries"] = json::array();
    std::ofstream out{"./config/client-struct.json"};
    out << std::setw(4) << temp << std::endl;
  }
}

void SyncStructure::read_structure() {

  if (entries.empty()) {
    std::ifstream i("./config/client-struct.json");
    std::unique_ptr<json> structure = std::make_unique<json>();
    i >> (*structure);
    for (size_t i = 0; i < (*structure)["entries"].size(); i++) {
      std::string path = (*structure)["entries"][i]["path"];
      json entry = (*structure)["entries"][i];
      entries[path] = entry;
    }
  }
}

/* PUBLIC SECTION */

std::shared_ptr<SyncStructure> SyncStructure::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<SyncStructure>(new SyncStructure{});
  }
  instance->create_structure();
  instance->read_structure();
  return instance;
}

void SyncStructure::write_structure() {
  DurationLogger duration{"WRITE_STRUCTURE"};
  std::unique_ptr<json> structure = std::make_unique<json>();
  if (entries.empty()) {
    (*structure)["hashed_status"] = std::string{"empty_hashed_status"};
    (*structure)["entries"] = json::array();
  } else {
    std::string entries_dump;
    for (auto it = entries.begin(); it != entries.end(); it++) {
      (*structure)["entries"].push_back(it->second);
      entries_dump += it->second.dump();
    }
    std::vector<char> data(entries_dump.begin(), entries_dump.end());
    (*structure)["hashed_status"] = (*structure)["entries"].empty()
                                        ? std::string{"empty_hashed_status"}
                                        : Sha256::getSha256(data);
  }
  std::ofstream o("./config/client-struct.json");
  o << (*structure) << "\n";
  o.close();
  (*structure).clear();
}

bool SyncStructure::has_entry(const std::string &path) {
  return entries.find(path) != entries.end();
}

size_t SyncStructure::get_last_mod(const std::string &path) {
  return entries[path]["last_mod"];
}

std::vector<std::string> SyncStructure::get_paths() {
  std::vector<std::string> paths{};
  for (auto it = entries.begin(); it != entries.end(); ++it) {
    paths.push_back(it->first);
  }
  return paths;
}

std::vector<std::string>
SyncStructure::get_entry_hashes(const std::string &path) {
  std::vector<std::string> hashes{};
  std::sort(entries[path]["chunks"].begin(), entries[path]["chunks"].begin(),
            [&](json c1, json c2) { return c1["id"] < c2["id"]; });
  for (size_t i = 0; i < entries[path]["chunks"].size(); i++) {
    hashes.push_back(entries[path]["chunks"]);
  }
  return hashes;
}

void SyncStructure::add_chunk(const json &chunk) {
  json chk = chunk["chunks"][0];
  if (entries.find(chunk["path"]) == entries.end()) {
    entries[chunk["path"]] = chunk;
  } else {
    entries[chunk["path"]]["chunks"].push_back(chk);
  }
}

void SyncStructure::replace_chunk(const json &chunk) {

  for (size_t i = 0; i < entries[chunk["path"]]["chunks"].size(); i++) {
    if (entries[chunk["path"]]["chunks"][i]["id"] == chunk["id"]) {
      entries[chunk["path"]]["chunks"][i] = chunk;
    }
  }
}

void SyncStructure::delete_chunk(const json &chunk) {

  size_t n_chunks = entries[chunk["path"]]["chunks"].size();
  for (size_t i = 0; i < n_chunks; i++) {
    size_t chk_id = entries[chunk["path"]]["chunks"][i]["id"];
    if (chk_id == chunk["id"]) {
      entries[chunk["path"]]["chunks"].erase(i);
    } else {
      if (chunk["id"] != (n_chunks - 1)) {
        entries[chunk["path"]]["chunks"][i]["id"] = chk_id - 1;
      }
    }
  }
}

void SyncStructure::delete_entry(const json &entry) {
  std::string path = entry["path"];
  entries.erase(path);
}

void SyncStructure::rename_entry(const json &entry) {
  json ent = entry;
  std::string old_path = entry["old_path"];
  std::string new_path = entry["path"];
  if (entries.find(old_path) != entries.end()) {
    ent["path"] = new_path;
    entries.erase(old_path);
  }
}
#include "../../include/filesystem/sync_structure.hpp"

/* PRIVATE SECTION */

void SyncStructure::read_structure() {
  std::unique_lock lk{m};
  DurationLogger logger{"READ_STRUCTURE"};
  if (std::filesystem::exists("./config/client-struct.json") ||
      !std::filesystem::is_empty("./config/client-struct.json")) {
    std::ifstream i("./config/client-struct.json");
    std::unique_ptr<json> structure = std::make_unique<json>();
    i >> (*structure);
    for (size_t i = 0; i < (*structure)["entries"].size(); i++) {
      std::string path = (*structure)["entries"][i]["path"];
      json entry = (*structure)["entries"][i];
      entries[path] = entry;
    }
    last_check = (*structure)["last_check"].get<int>();
  }
}

/* PUBLIC SECTION */

void SyncStructure::write_structure() {
  std::unique_ptr<json> structure = std::make_unique<json>();
  DurationLogger duration{"WRITE_STRUCTURE"};
  if (entries.empty()) {
    (*structure)["entries"] = json::array();
    (*structure)["last_check"] = 0;
  } else {
    std::string entries_dump;
    for (auto it = entries.begin(); it != entries.end(); it++) {
      (*structure)["entries"].push_back(it->second);
      entries_dump += it->second.dump();
    }
    last_check = (int)std::time(nullptr);
    (*structure)["last_check"] = last_check;
  }
  std::ofstream o("./config/client-struct.json");
  o << (*structure) << "\n";
  o.close();
}

int SyncStructure::get_last_check() { return last_check; }

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
  DurationLogger logger{"ADD_CHUNK"};
  json chk = chunk["chunks"][0];
  if (entries.find(chunk["path"]) == entries.end()) {
    entries[chunk["path"]] = chunk;
  } else {
    entries[chunk["path"]]["chunks"].push_back(chk);
  }
  if (entries[chunk["path"]]["chunks"].size() ==
      (size_t)chunk["nchunks"].get<int>()) {
    std::string file_hash{""};
    for (size_t i = 0; i < entries[chunk["path"]]["chunks"].size(); i++) {
      file_hash += entries[chunk["path"]]["chunks"][i]["hash"];
      std::shared_ptr<char[]> buff{new char[file_hash.size()]};
      for (size_t j = 0; j < file_hash.size(); j++) {
        buff.get()[j] = file_hash[j];
      }
      file_hash = Sha256::getSha256(buff, file_hash.size());
    }
    entries[chunk["path"]]["hash"] = file_hash;
    entries[chunk["path"]]["chunks"].clear();
  }
}

void SyncStructure::delete_entry(const json &entry) {
  DurationLogger logger{"DELETE_ENTRY"};
  std::string path = entry["path"];
  entries.erase(path);
}

void SyncStructure::rename_entry(const json &entry) {
  DurationLogger logger{"RENAME_ENTRY"};
  json ent = entry;
  std::string old_path = entry["old_path"];
  std::string new_path = entry["path"];
  if (entries.find(old_path) != entries.end()) {
    ent["path"] = new_path;
    entries.erase(old_path);
  }
}
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
  if (structure.get() == nullptr || (*structure).empty()) {
    structure = std::make_unique<json>();
    std::ifstream i("./config/client-struct.json");
    i >> (*structure);
    if (!(*structure)["entries"].empty()) {
      for (size_t i = 0; i < (*structure)["entries"].size(); i++) {
        json tmp = (*structure)["entries"][i];
        entries[tmp["path"]] = std::make_tuple((int)i, tmp);
        count++;
      }
    }
  }
}

void SyncStructure::hash_struct() {
  DurationLogger duration{"HASH_STRUCTURE"};
  if ((*structure)["entries"].empty()) {
    (*structure)["hashed_status"] = std::string{"empty_hashed_status"};
  } else {
    std::string entries_dump = (*structure)["entries"].dump();
    std::vector<char> data(entries_dump.begin(), entries_dump.end());
    (*structure)["hashed_status"] = (*structure)["entries"].empty()
                                        ? std::string{"empty_hashed_status"}
                                        : Sha256::getSha256(data);
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
  instance->hash_struct();
  std::ofstream o("./config/client-struct.json");
  o << (*structure) << "\n";
  o.close();
  (*structure).clear();
}

bool SyncStructure::has_entry(const std::string &path) {
  return entries.find(path) != entries.end();
}

size_t SyncStructure::get_last_mod(const std::string &path) {
  int index = std::get<0>(entries[path]);
  return (*structure)["entries"][index]["last_mod"];
}

std::vector<std::string> SyncStructure::get_paths() const {
  std::vector<std::string> paths;
  for (const json &entry : (*structure)["entries"]) {
    paths.push_back(entry["path"]);
  }
  return paths;
}

void SyncStructure::add_chunk(const json &chunk) {
  json chk = chunk["chunks"][0];
  std::clog << "chunk: " << chk.dump() << "\n";
  std::clog << "chunk dump: " << chunk.dump() << "\n";
  if (entries.find(chunk["path"]) == entries.end()) {
    std::clog << "olga\n";
    (*structure)["entries"].push_back(chunk);
    entries[chunk["path"]] = std::make_tuple(count++, chunk);
  } else {
    int index = std::get<0>(entries[chunk["path"]]);
    (*structure)["entries"][index]["chunks"].push_back(chk);
  }
}

void SyncStructure::replace_chunk(const json &chunk) {
  int index = std::get<0>(entries[chunk["path"]]);
  for (size_t i = 0; i < (*structure)["entries"][index]["chunks"].size(); i++) {
    if ((*structure)["entries"][index]["chunks"][i]["id"] == chunk["id"]) {
      (*structure)["entries"][index]["chunks"][i] = chunk;
    }
  }
}

void SyncStructure::delete_chunk(const json &chunk) {
  int index = std::get<0>(entries[chunk["path"]]);
  size_t n_chunks = (*structure)["entries"][index]["chunks"].size();
  for (size_t i = 0; i < n_chunks; i++) {
    size_t chk_id = (*structure)["entries"][index]["chunks"][i]["id"];
    if (chk_id == chunk["id"]) {
      (*structure)["entries"][index]["chunks"].erase(i);
    } else {
      if (chunk["id"] != (n_chunks - 1)) {
        (*structure)["entries"][index]["chunks"][i]["id"] = chk_id - 1;
      }
    }
  }
}

void SyncStructure::delete_entry(const json &entry) {
  int index = std::get<0>(entries[entry["path"]]);
  entries.erase(entry["path"]);
  (*structure)["entries"].erase(index);
  count--;
}

void SyncStructure::rename_entry(const json &entry) {
  std::string old_path = entry["old_path"];
  std::string new_path = entry["path"];
  if (entries.find(old_path) != entries.end()) {
    int index = std::get<0>(entries[old_path]);
    json entry = std::get<1>(entries[old_path]);
    entry["path"] = new_path;
    (*structure)["entries"][index]["path"] = new_path;
    entries[new_path] = std::make_tuple(index, entry);
    entries.erase(old_path);
  }
}
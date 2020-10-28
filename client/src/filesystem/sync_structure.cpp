#include "../../include/filesystem/sync_structure.hpp"

void SyncStructure::read_structure() {
  DurationLogger logger{"READ_STRUCTURE"};
  if (std::filesystem::exists("./config/client-struct.json") &&
      !std::filesystem::is_empty("./config/client-struct.json")) {
    std::ifstream i("./config/client-struct.json");
    std::unique_ptr<json> structure = std::make_unique<json>();
    i >> (*structure);
    for (size_t i = 0; i < (*structure)["entries"].size(); i++) {
      std::string path = (*structure)["entries"][i]["path"];
      json entry = (*structure)["entries"][i];
      (*entries)[path] = entry;
    }
    last_check = (*structure)["last_check"].get<int>();
  } else
    throw FileStructNotValid();
}

void SyncStructure::write_structure() {
  std::unique_ptr<json> structure = std::make_unique<json>();
  DurationLogger duration{"WRITE_STRUCTURE"};
  if ((*entries).empty()) {
    (*structure)["entries"] = json::array();
  } else {
    std::string entries_dump;
    for (auto it = (*entries).begin(); it != (*entries).end(); it++) {
      (*structure)["entries"].push_back(it->second);
      entries_dump += it->second.dump();
    }
  }
  last_check = (int)std::time(nullptr);
  (*structure)["last_check"] = last_check;
  std::ofstream o("./config/client-struct.json");
  o << (*structure) << "\n";
  o.close();
}

int SyncStructure::get_last_check() { return last_check; }

bool SyncStructure::has_path(const std::string &path) {
  return (*entries).find(path) != (*entries).end();
}

std::vector<std::string> SyncStructure::get_paths() {
  std::vector<std::string> paths{};
  for (auto it = (*entries).begin(); it != (*entries).end(); ++it) {
    paths.push_back(it->first);
  }
  return paths;
}

void SyncStructure::add_chunk(const json &chunk) {
  DurationLogger logger{"ADD_CHUNK"};
  json chk = chunk["transfers"]["chunks"][0];
  if ((*entries).find(chunk["path"]) == (*entries).end()) {
    (*entries)[chunk["path"]] = chunk;
  } else {
    (*entries)[chunk["path"]]["transfers"]["chunks"].push_back(chk);
  }
  if ((*entries)[chunk["path"]]["transfers"]["chunks"].size() ==
      (size_t)chunk["transfers"]["nchunks"].get<int>()) {
    (*entries)[chunk["path"]].erase("transfers");
    std::clog << "struct new file: " << (*entries)[chunk["path"]].dump()
              << "\n";
  }
}

void SyncStructure::delete_entry(const json &entry) {
  DurationLogger logger{"DELETE_ENTRY"};
  std::string path = entry["path"];
  (*entries).erase(path);
}

std::vector<json> SyncStructure::get_incomplete_entries() const {
  std::vector<json> result{};
  for (const auto &[key, value] : (*entries)) {
    if (value.find("chunks") != value.end())
      result.push_back(value);
  }
  return result;
}

int SyncStructure::get_last_local_change(const std::string &path) const {
  return (*entries)[path]["last_local_change"].get<int>();
}

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>

#include "../common/singleton.hpp"
#include "../common/duration.hpp"
#include "../common/json.hpp"
#include "../common/sha256.hpp"

using json = nlohmann::json;

class SyncStructure: public Singleton<SyncStructure> {

private:
  friend class Singleton;
  std::unique_ptr<std::unordered_map<std::string, json>> entries;
  int last_check = 0;
  
  SyncStructure(){
    entries = std::make_unique<std::unordered_map<std::string, json>>();
    read_structure();
  }
  
  void read_structure();
  void write_structure();

public:
  ~SyncStructure() {
    write_structure();
  }
  
  void add_chunk(const json &chunk);
  void delete_entry(const json &entry);
  void rename_entry(const json &entry);
  std::vector<std::string> get_paths();
  int get_last_check();
};
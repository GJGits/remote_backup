#pragma once

#include "../common/json.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>

using json = nlohmann::json;

class HandleWatcher {
  inline static HandleWatcher *instance = nullptr;
public:
  static HandleWatcher *getInstance() {
    if (instance == nullptr) {
      instance = new HandleWatcher();
    }
    return instance;
  }

  json hash_struct(json &structure);

  json read_structure();

  void write_structure(const json &structure);

  void handle_InCreate(std::string);

  /*File was modified (e.g., write(2), truncate(2)).*/
  void handle_InModify(std::string);

  /*File/directory deleted from watched directory.*/
  void handle_InDelete(std::string);

  /*     IN_MOVED_FROM (+)
                Generated for the directory containing the old filename
                when a file is renamed.

         IN_MOVED_TO (+)
                Generated for the directory containing the new filename
                when a file is renamed.*/
  void handle_InRename(std::string, std::string);

  // manca l'erros sul file watcher che non  so come monitorare
};

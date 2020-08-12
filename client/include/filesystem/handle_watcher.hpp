#pragma once

#include "../common/json.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <future>

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

  void handle_InCloseWrite(const std::string&); // true è file, false è cartella

  /*File was modified (e.g., write(2), truncate(2)).*/
  void handle_InModify(std::string);

  /*File/directory deleted from watched directory.*/
  void handle_InDelete(std::string);

  void handle_prune();

  void handle_expand(const std::string &path);

  /*     IN_MOVED_FROM (+)
                Generated for the directory containing the old filename
                when a file is renamed.

         IN_MOVED_TO (+)
                Generated for the directory containing the new filename
                when a file is renamed.*/
  void handle_InRename(const std::string &);

  // manca l'erros sul file watcher che non  so come monitorare
  
  void handle_InMove(std::string);
  
};

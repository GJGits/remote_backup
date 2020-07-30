//
// Created by alessandra on 30/07/20.
//

#ifndef REMOTE_BACKUP_HANDLE_WATCHER_H
#define REMOTE_BACKUP_HANDLE_WATCHER_H

#include "unistd.h"
#include <errno.h>
#include <iostream>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/inotify.h>
#include <unistd.h>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <regex>

class Handle_watcher {
  inline static Handle_watcher *instance = nullptr;
public:
  static Handle_watcher *getInstance() {
    if (instance == nullptr) {
      instance = new Handle_watcher();
    }
    return instance;
  }
  // eventi da monitorare

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

#endif // REMOTE_BACKUP_HANDLE_WATCHER_H

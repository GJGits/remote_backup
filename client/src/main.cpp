#include "unistd.h"
#include <iostream>
#include "../include/filesystem/directory.hpp"
#include "../include/filesystem/file.hpp"
#include "../include/filesystem/linux_watcher.hpp"

int main() {

  std::shared_ptr<Directory> root = Directory::getRoot();
  std::shared_ptr<Directory> last_parent_dir{nullptr};

  for (auto &p : std::filesystem::recursive_directory_iterator("./sync")) {
    std::stringstream ss(p.path().string());
    std::vector<std::string> tokens;
    std::string token;

    // collecting tokens
    while (std::getline(ss, token, '/')) {
      if (!token.empty()) {
        tokens.push_back(token);
      }
    }

    // create structure
    for (std::vector<int>::size_type i = 0; i != tokens.size(); i++) {
      // sicuramente una directory
      if (i < tokens.size() - 1) {
        // se parent dir e' vuota allora sono nella root
        if (last_parent_dir.get() == nullptr) {
          // se children non contiene la directory aggiungo
          last_parent_dir = root->getDir(tokens[i]).lock().use_count() == 0
                                ? root->addDirectory(tokens[i])
                                : root->getDir(tokens[i]).lock();
        } else {
          last_parent_dir =
              last_parent_dir->getDir(tokens[i]).lock().use_count() == 0
                  ? last_parent_dir->addDirectory(tokens[i])
                  : last_parent_dir->getDir(tokens[i]).lock();
        }
      }
    }

    if (p.is_directory()) {
      last_parent_dir->addDirectory(p.path().filename().string());
    }

    if (p.is_regular_file()) {
      last_parent_dir->addFile(p.path().filename().string(), p.file_size());
    }

    last_parent_dir = std::shared_ptr<Directory>{nullptr};
  }

  root->ls(0);

  LinuxWatcher *watcher = LinuxWatcher::getInstance(
      "./sync", IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_TO);
  watcher->handle_events();

  return 0;
}
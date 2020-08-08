#include "../include/filesystem/linux_watcher.hpp"
#include "../include/filesystem/sync_structure.hpp"
#include <iostream>

int main() {

  /* START WATCHER */
  SyncStructure *structure = SyncStructure::getInstance();
  LinuxWatcher *watcher = LinuxWatcher::getInstance(
      "./sync", IN_ONLYDIR | IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_TO |
                    IN_MOVED_FROM | IN_ISDIR);
  for (auto &p : std::filesystem::recursive_directory_iterator("./sync")) {
    if (p.is_directory()) {
      watcher->add_watch(p.path().string());
    } else {
      structure->add_entry(p.path().string());
    }
  }
  watcher->handle_events();

  return 0;
}

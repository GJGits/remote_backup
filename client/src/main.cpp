#include "../include/filesystem/linux_watcher.hpp"
#include <iostream>

int main() {

  /* START WATCHER */
  LinuxWatcher *watcher = LinuxWatcher::getInstance("./sync");
  for (auto &p : std::filesystem::recursive_directory_iterator("./sync")) {
    if (p.is_directory()) {
      watcher->add_watch(p.path().string(), IN_ONLYDIR | IN_CREATE | IN_DELETE |
                                                IN_MODIFY | IN_MOVED_TO | IN_MOVED_FROM | IN_ISDIR);
    }
  }
  watcher->handle_events();

  return 0;
}

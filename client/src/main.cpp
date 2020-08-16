#include "../include/filesystem/linux_watcher.hpp"
#include "../include/filesystem/sync_structure.hpp"
#include <iostream>

int main() {

  /* START WATCHER */
  LinuxWatcher *watcher = LinuxWatcher::getInstance(
      "./sync", IN_CREATE | IN_ONLYDIR | IN_CLOSE_WRITE | IN_DELETE |
                    IN_MODIFY | IN_MOVED_TO | IN_MOVED_FROM | IN_ISDIR);

     watcher->scan();
     watcher->handle_events();

  return 0;
}

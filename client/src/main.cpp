#include "unistd.h"
#include <iostream>

#include "../include/common/linux_watcher.hpp"

int main() {

  //LinuxWatcher watcher{"./sync", IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_TO};
  LinuxWatcher *watcher=LinuxWatcher::getInstance("./sync", IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_TO);
  watcher->handle_events();

 return 0;
}
#include "unistd.h"
#include <iostream>

#include "../include/common/linux_watcher.hpp"

int main() {

  LinuxWatcher watcher{"./sync", IN_OPEN | IN_CLOSE};  
  watcher.handle_events();

 return 0;
}
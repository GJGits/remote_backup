#include <iostream>

#include "../include/filesystem/linux_watcher.hpp"
#include "../include/modules/gui_module.hpp"
#include "../include/pubsub/struct_sub.hpp"
#include "../include/pubsub/sync_sub.hpp"

int main() {
  sleep(4);
  std::shared_ptr<StructSubscriber> struct_sub =
      StructSubscriber::getInstance();
  struct_sub->init();

  std::shared_ptr<SyncSubscriber> sync_sub = SyncSubscriber::getInstance();
  sync_sub->init_sub_list();

  std::shared_ptr<GuiModule> gui_module = GuiModule::getInstance();
  gui_module->init_sub_list();
  gui_module->start();

  std::shared_ptr<LinuxWatcher> watcher = LinuxWatcher::getInstance(
      "./sync", IN_CREATE | IN_ONLYDIR | IN_CLOSE_WRITE | IN_DELETE |
                    IN_MODIFY | IN_MOVED_TO | IN_MOVED_FROM | IN_ISDIR);

  watcher->init_sub_list();

  return 0;
}

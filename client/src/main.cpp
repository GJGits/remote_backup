#include <csignal>
#include <iostream>

#include "../include/filesystem/linux_watcher.hpp"
#include "../include/modules/gui_module.hpp"
#include "../include/pubsub/struct_sub.hpp"
#include "../include/pubsub/sync_sub.hpp"

void signalHandler(int signum) {
  // cleanup and close up stuff here
  // terminate program
  std::shared_ptr<StructSubscriber> struct_sub =
      StructSubscriber::getInstance();
  std::shared_ptr<SyncSubscriber> sync_sub = SyncSubscriber::getInstance();
  std::shared_ptr<GuiModule> gui_module = GuiModule::getInstance();
  struct_sub->stop(Message{TOPIC::CLOSE});
  gui_module->stop(Message{TOPIC::CLOSE});
  sync_sub->stop(Message{TOPIC::CLOSE});
  exit(signum);
}

int main() {

  // register signal SIGINT and signal handler
  signal(SIGTERM, signalHandler);

  std::shared_ptr<StructSubscriber> struct_sub =
      StructSubscriber::getInstance();
  struct_sub->init_sub_list();

  std::shared_ptr<SyncSubscriber> sync_sub = SyncSubscriber::getInstance();
  sync_sub->init_sub_list();

  std::shared_ptr<LinuxWatcher> watcher = LinuxWatcher::getInstance(
      "./sync", IN_CREATE | IN_ONLYDIR | IN_DELETE | IN_MODIFY | IN_MOVED_TO |
                    IN_MOVED_FROM | IN_ISDIR | IN_IGNORED);

  watcher->init_sub_list();

  std::shared_ptr<GuiModule> gui_module = GuiModule::getInstance();
  gui_module->init_sub_list();

  // azione bloccante, questo modulo rimane in vita per tutta
  // la durata dell'applicazione, idealmente infatti e' il client
  // tramite interfaccia a comunicare la chiusura.
  gui_module->start();

  return 0;
}

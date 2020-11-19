#include <csignal>
#include <iostream>
#include "../include/filesystem/linux_watcher.hpp"
#include "../include/modules/gui_module.hpp"
#include "../include/pubsub/sync_sub.hpp"

// init commit-m

void signalHandler(int signum) {
  // cleanup and close up stuff here
  // terminate program
  std::shared_ptr<GuiModule> gui_module = GuiModule::getInstance();
  gui_module->close();
  exit(signum);
}

int main() {

  // register signal SIGINT and signal handler
  signal(SIGTERM, signalHandler);

  std::shared_ptr<SyncSubscriber> sync_sub = SyncSubscriber::getInstance();
  sync_sub->init_sub_list();

  std::shared_ptr<LinuxWatcher> watcher = LinuxWatcher::getInstance();
  watcher->init_sub_list();

  std::shared_ptr<GuiModule> gui_module = GuiModule::getInstance();
  gui_module->init_sub_list();

  // registro moduli
  gui_module->add_module(sync_sub);
  gui_module->add_module(watcher);

  // azione bloccante, questo modulo rimane in vita per tutta
  // la durata dell'applicazione, idealmente infatti e' il client
  // tramite interfaccia a comunicare la chiusura.
  gui_module->start();

  return 0;
}

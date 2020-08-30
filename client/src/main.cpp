#include "../include/filesystem/linux_watcher.hpp"
#include "../include/filesystem/struct_worker.hpp"
#include "../include/filesystem/sync_structure.hpp"
#include "../include/filesystem/sync_worker.hpp"
#include <iostream>

int main() {

  std::clog << "Using Boost "     
          << BOOST_VERSION / 100000     << "."  // major version
          << BOOST_VERSION / 100 % 1000 << "."  // minor version
          << BOOST_VERSION % 100                // patch level
          << "\n";

  StructWorker *struct_worker = StructWorker::getInstance();
  SyncWorker *sync_worker = SyncWorker::getInstance();
  HandleWatcher *dispatcher = HandleWatcher::getInstance();
  LinuxWatcher *watcher = LinuxWatcher::getInstance(
      "./sync", IN_CREATE | IN_ONLYDIR | IN_CLOSE_WRITE | IN_DELETE |
                    IN_MODIFY | IN_MOVED_TO | IN_MOVED_FROM | IN_ISDIR);

  struct_worker->run_worker();
  sync_worker->run_sync_worker();
  sync_worker->run_up_workers();
  sync_worker->run_down_workers();
  dispatcher->run_dispatcher();
  watcher->handle_events();

  return 0;
}

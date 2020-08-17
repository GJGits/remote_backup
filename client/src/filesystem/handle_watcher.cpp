//
// Created by alessandra on 30/07/20.
//

#include "../../include/filesystem/handle_watcher.hpp"
#include "../../include/filesystem/linux_watcher.hpp"
#include "../../include/filesystem/sync_structure.hpp"

HandleWatcher *HandleWatcher::getInstance() {
  if (instance == nullptr) {
    instance = new HandleWatcher();
    instance->increase_counter(); // serve per prima scan
    instance->run_worker();
  }
  return instance;
}

HandleWatcher::~HandleWatcher() {
  finish = true;
  worker.join();
  for (int i = 0; i < 4; i++) {
    senders[i].join();
  }
}

void HandleWatcher::run_worker() {

  worker = std::move(std::thread{[&]() {
    while (!finish) {

      std::clog << "counter = " << getInstance()->get_count() << "\n";
      std::unique_lock lk{m};

      if (!events.empty()) {

        try {

          Event event = events.front();
          events.pop();

          switch (event.getType()) {

          case EVENT_TYPE::CREATE:
            handle_InCloseWrite(event.getArgument1());
            break;

          case EVENT_TYPE::EXPAND:
            handle_expand(event.getArgument1());
            break;

          case EVENT_TYPE::MOVED:
            getInstance()->increase_counter();
            break;

          case EVENT_TYPE::DELETE:
            handle_InDelete(event.getArgument1());
            break;

          case EVENT_TYPE::RENAME:
            if (event.getArgument1().empty()) {
              handle_InCloseWrite(event.getArgument2());
            } else {
              handle_InRename(event.getArgument1(), event.getArgument2());
              getInstance()->decrease_counter();
            }

            break;

          case EVENT_TYPE::PRUNING:
            if (getInstance()->get_count() > 0) {
              handle_prune();
              getInstance()->reset_counter();
            }

            break;

          default:
            break;
          }

        } catch (std::filesystem::__cxx11::filesystem_error &e) {
          std::clog << e.what() << "\n";
        } catch (...) {
          std::clog << "Errore sconosciuto\n";
        }
      } else {
        cv.wait(lk, [&]() { return !events.empty() || finish; });
      }
    }
  }});
}

void HandleWatcher::run_senders() {
  for (int i = 0; i < 4; i++) {
    senders.emplace_back([&]() {
      while (!finish) {
        FileEntry entry;
        {
          std::unique_lock lk{m1};
          if (!file_entries.empty()) {
            entry = file_entries.front();
            file_entries.pop();
          } else {
            cv1.wait(lk, [&]() { return !file_entries.empty() || finish; });
          }
        }
        // todo: job;
      }
    });
  }
}

void HandleWatcher::push_event(const Event &event) {
  std::unique_lock lk{m};
  events.push(event);
  cv.notify_one();
}

void HandleWatcher::handle_InCloseWrite(const std::string &path) {
  std::clog << " Evento: InCloseWrite , path : " << path << "\n";

  if (std::filesystem::exists(path)) {
    SyncStructure *sync = SyncStructure::getInstance();
    FileEntry entry = std::move(sync->add_entry(path));
    sync->write_structure();
    file_entries.push(entry);
    cv1.notify_one();
  }
}

void HandleWatcher::handle_expand(const std::string &path) {
  std::clog << " Evento: expand , path : " << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
    if (std::filesystem::is_regular_file(p.path().string())) {
      FileEntry entry = std::move(sync->add_entry(p.path().string()));
      file_entries.push(entry);
      cv1.notify_one();
    }
  }
  sync->write_structure();
}

void HandleWatcher::handle_InDelete(const std::string &path) {
  std::clog << " Evento: InDelete, path :" << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  sync->remove_entry(path);
  sync->write_structure();
  // todo: send delete to server
}

void HandleWatcher::handle_prune() {
  std::clog << " Evento: Pruning\n";
  SyncStructure *sync = SyncStructure::getInstance();
  std::vector<std::string> deleted = sync->prune();
  sync->write_structure();
  if (!deleted.empty()) {
    // todo invia al server
  }
}

void HandleWatcher::handle_InModify(const std::string &path) {
  std::clog << " Evento: InModify, path :" << path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();
  FileEntry entry = std::move(sync->add_entry(path));
  file_entries.push(entry);
  cv1.notify_one();
  sync->write_structure();
}

void HandleWatcher::handle_InRename(const std::string &old_path,
                                    const std::string &new_path) {
  std::clog << " Evento: InRename, old_path :" << old_path
            << ", new_path: " << new_path << "\n";
  SyncStructure *sync = SyncStructure::getInstance();

  sync->rename_entry(old_path, new_path);
  sync->write_structure();
}

void HandleWatcher::handle_InMove(const std::string &path) {
  std::clog << " Evento: InMove, old_path =" << path << "\n";
}

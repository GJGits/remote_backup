#pragma once
#include "../common/duration.hpp"
#include "handle_watcher.hpp"
#include "sync_structure.hpp"
#include <bits/stdc++.h>
#include <chrono>
#include <errno.h>
#include <iostream>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/inotify.h>
#include <unistd.h>
#include <unordered_map>

/**
 * Classe singleton che racchiude un metodo statico per ogni evento da
 * monitorare
 **/
class LinuxWatcher {
private:
  int timer;
  int inotify_descriptor;
  std::string root_to_watch;
  uint32_t watcher_mask;
  std::unordered_map<std::string, int> path_wd_map;
  std::unordered_map<int, std::string> wd_path_map;
  std::unordered_map<int, std::string> cookie_map;
  inline static LinuxWatcher *instance = nullptr;
  LinuxWatcher(const std::string &root_to_watch, uint32_t mask)
      : watcher_mask{mask} {
    timer = -1;
    // Richiedo un file descriptor al kernel da utilizzare
    // per la watch. L'API fornita prevede, come spesso avviene
    // in linux, che la comunicazione tra kernel e user avvenga
    // tramite lettura da "file".
    inotify_descriptor = inotify_init1(IN_NONBLOCK);
    // Fatal error, in questo caso il kernel non e' in grado di
    // restituire un file descriptor, non possiamo proseguire.
    if (inotify_descriptor == -1) {
      perror("inotify_init1");
      exit(-1);
    }
  }

public:
  static LinuxWatcher *getInstance(const std::string &root_path,
                                   uint32_t mask) {
    if (instance == nullptr) {
      instance = new LinuxWatcher{root_path, mask};
      instance->add_watch(root_path);
    }
    return instance;
  }

  ~LinuxWatcher() {
    // Quando l'oggetto viene distrutto rilascio il file descriptor
    // in questo modo il kernel ha la possibilità di riassegnarlo ad
    // un altro processo.
    close(inotify_descriptor);
  }

  /**
   * Permette di aggiungere o modificare un watch all'inotify descriptor.
   *
   * @path: path relativo alla cartella o al file da monitorare.
   * @mask: 4 byte per descrivere il codice di evento da monitorare
   *
   * @return: true se l'evento e' stato aggiunto alla lista del monitoring,
   *          false altrimenti.
   */
  bool add_watch(const std::string &path) {
    int wd = inotify_add_watch(inotify_descriptor, path.c_str(), watcher_mask);
    if (wd == -1)
      return false;
    path_wd_map[path] = wd;
    wd_path_map[wd] = path;
    for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
      if (p.is_directory()) {
        add_watch(p.path().string());
      }
    }
    return true;
  }

  /**
   * Dato il path di un file o di una cartella permette di rimuovere il
   * watching precedentemente creato.
   *
   * @path: path relativo al file o directory di cui si vuole smettere di
   *        ricevere notifiche.
   * @return: true in caso di successo, false altrimenti.
   */
  bool remove_watch(const std::string &path) {
    if (path_wd_map.find(path) == path_wd_map.end() ||
        path.compare(root_to_watch) == 0)
      return false;
    inotify_rm_watch(inotify_descriptor, path_wd_map[path]);
    wd_path_map.erase(path_wd_map[path]);
    path_wd_map.erase(path);
    return true;
  }

  /**
   * Effettua handle degli eventi per poi rigirarli ad un apposito
   * event handler.
   */
  void handle_events() {

    std::clog << "Start monitoring...\n";
    for (;;) {
      std::clog << "Wating for an event...\n";
      // Some systems cannot read integer variables if they are not
      // properly aligned. On other systems, incorrect alignment may
      // decrease performance. Hence, the buffer used for reading from
      // the inotify file descriptor should have the same alignment as
      // struct inotify_event.

      char buf[4096]
          __attribute__((aligned(__alignof__(struct inotify_event))));
      memset(buf, '\0', 4096);
      const struct inotify_event *event;
      ssize_t len;
      char *ptr; // ptr per consumare il buffer

      // Puo' questo blocco essere spostato fuori dal for per singola
      // inizializzazione?
      int poll_num;
      nfds_t nfds = 1;
      struct pollfd fds[1];
      fds[0].fd = inotify_descriptor;
      fds[0].events = POLLIN; // maschera generale per eventi su fd?

      // poll until an event occurs.Timeout = -1 -> BLOCKING,
      // else timeout expressed in milliseconds
      HandleWatcher *handlewatcher = HandleWatcher::getInstance();
      poll_num = poll(fds, nfds, timer);
      if (poll_num > 0) {

        len = read(inotify_descriptor, buf, sizeof buf);
        // todo: check su read qui...

        for (ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len) {

          DurationLogger duration{"HANDLING_EVENT"};

          event = (const struct inotify_event *)ptr;

          std::string full_path =
              wd_path_map[event->wd] + "/" + std::string{event->name};

          try {

            std::clog << "MASK: " << event->mask << "\n";

            //
            //  TABELLA RIASSUNTIVA CODICI EVENTI:

            //    8          -> EVENTO IN_CLOSE_WRITE
            //    1073741952 -> CTRL+Z CARTELLA ?
            //    1073742080 -> EVENT HANDLE_EXPAND
            //    1073741888 -> MOVED_FROM FOLDER
            //    64         -> MOVED_FROM FILE
            //    128        -> RENAME  (MOVED_TO)
            //    512        -> IN_DELETE

            switch (event->mask) {
            case 8:
              timer = 5000;
              handlewatcher->push_event(Event(EVENT_TYPE::CREATE, full_path));
              break;
            case 1073742080:
              add_watch(full_path);
              handlewatcher->push_event(Event(EVENT_TYPE::EXPAND, full_path));
              break;
            case 1073741952:
              add_watch(full_path);
              handlewatcher->push_event(Event(EVENT_TYPE::EXPAND, full_path));
              break;
            case 1073741888:
              timer = 5000;
              remove_watch(full_path);
              handlewatcher->push_event(Event(EVENT_TYPE::MOVED));
              break;
            case 64:
              timer = 5000;
              cookie_map[event->cookie] = full_path;
              // todo: eliminare con pruning entry da cookie_map non associate a
              // moved_to
              handlewatcher->push_event(Event(EVENT_TYPE::MOVED));
              break;
            case 128:
              timer = 5000;
              handlewatcher->push_event(Event(
                  EVENT_TYPE::RENAME, cookie_map[event->cookie], full_path));
              cookie_map.erase(event->cookie);
              break;
            case 512:
              timer = 5000;
              handlewatcher->push_event(Event(EVENT_TYPE::DELETE, full_path));
              break;
            default:
              break;
            }

          }

          catch (const std::filesystem::filesystem_error &e) {

            std::clog << e.what() << "\n";
          }
        }
      }
      if (poll_num == 0) {
        cookie_map.clear();
        timer = -1;
      }
    }
  }
};

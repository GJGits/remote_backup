#pragma once
#include "handle_watcher.hpp"
#include <chrono>
#include <condition_variable>
#include <errno.h>
#include <future> // std::async, std::future
#include <iostream>
#include <mutex>
#include <poll.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/inotify.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

/**
 * Classe singleton che racchiude un metodo statico per ogni evento da
 * monitorare
 **/
class LinuxWatcher {
private:
  int inotify_descriptor;
  std::string root_to_watch;
  uint32_t watcher_mask;
  std::thread garbage_collector;
  std::queue<int> mod_from_cookies;
  std::unordered_map<std::string, int> path_wd_map;
  std::unordered_map<int, std::string> wd_path_map;
  std::unordered_map<int, std::string> cookie_map;
  inline static LinuxWatcher *instance = nullptr;
  LinuxWatcher(const std::string &root_to_watch, uint32_t mask)
      : watcher_mask{mask} {
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
  std::mutex m;
  std::condition_variable cv;
  std::queue<int> getCookieQueue() { return mod_from_cookies; }
  std::unordered_map<int, std::string> getCookieMap() { return cookie_map; }

  void garbage_collect() {
    std::clog << "size: " << instance->getCookieQueue().size() << "\n";
    int cookie = mod_from_cookies.front();
    // se il cookie e' ancora nella mappa allora
    // una momoved_to NON E' STATA CHIAMATA quindi
    // FORSE e' da eliminare, per assicurarsi che
    // sia da eliminare faccio piu' tentativi prima
    // di procedere.
    int tentativi = 3;
    while (tentativi > 0) {
      if (cookie_map.find(cookie) != cookie_map.end()) {
        tentativi--;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      } else {
        break;
      }
    }
    if (tentativi == 0) {
      HandleWatcher *handlewatcher = HandleWatcher::getInstance();
      // todo: eliminare entry da struttura e non cercare da filesystem perche'
      //       i file non esistono piu'
      for (auto &p :
           std::filesystem::recursive_directory_iterator(cookie_map[cookie])) {
        if (!p.is_directory()) {
          handlewatcher->handle_InDelete(p.path().string());
        }
      }
      cookie_map.erase(cookie);
    }
    mod_from_cookies.pop();
  }

  void run_garbage_collector() {
    instance->garbage_collector = std::move(std::thread([&]() {
      while (1) {
        std::unique_lock<std::mutex> lk{instance->m};
        if (instance->getCookieQueue().empty()) {
          instance->cv.wait(
              lk, [&]() { return !instance->getCookieQueue().empty(); });
        }
        if (instance->getCookieQueue().size() > 0) {
          garbage_collect();
        }
      }
    }));
  }

  static LinuxWatcher *getInstance(const std::string &root_path,
                                   uint32_t mask) {
    if (instance == nullptr) {
      instance = new LinuxWatcher{root_path, mask};
      instance->add_watch(root_path);
      instance->run_garbage_collector();
    }
    return instance;
  }

  ~LinuxWatcher() {
    // Quando l'oggetto viene distrutto rilascio il file descriptor
    // in questo modo il kernel ha la possibilità di riassegnarlo ad
    // un altro processo.
    close(inotify_descriptor);
    garbage_collector.join();
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

      // poll until an event occurs (timeout = -1 -> BLOCKING)
      poll_num = poll(fds, nfds, -1);
      if (poll_num > 0) {

        len = read(inotify_descriptor, buf, sizeof buf);
        // todo: check su read qui...

        HandleWatcher *handlewatcher = HandleWatcher::getInstance();

        for (ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len) {

          event = (const struct inotify_event *)ptr;

          std::string full_path =
              wd_path_map[event->wd] + "/" + std::string{event->name};

          std::clog << "full_path: " << full_path << "\n";

          try {

            if (event->mask & IN_CREATE) {
              if (event->mask & IN_ISDIR)
                add_watch(full_path);
              else
                handlewatcher->handle_InCreate(full_path);
            }

            if (event->mask & IN_DELETE) {
              if (event->mask & IN_ISDIR) {
                bool res = remove_watch(full_path);
                std::clog << "delete res = " << res << "\n";
                for (auto &p :
                     std::filesystem::recursive_directory_iterator(full_path)) {
                  if (!p.is_directory()) {
                    handlewatcher->handle_InDelete(p.path().string());
                  }
                }
              } else
                handlewatcher->handle_InDelete(full_path);
            }

            if (event->mask & IN_MODIFY)
              handlewatcher->handle_InModify(full_path);

            // L'evento in basso, capisce che è stato eliminato un file da GUI
            // Una MOVED_FROM senza MOVED_TO è una cancellazione, mentre con
            // MOVED_TO è rinominazione

            if (event->mask & IN_MOVED_FROM) {
              std::clog << "SONO in moved from \n";
              cookie_map[event->cookie] = std::string{event->name};
              std::unique_lock lk{m};
              instance->mod_from_cookies.push(event->cookie);
              cv.notify_all();
            }
            if (event->mask & IN_MOVED_TO) {
              // todo: se ripristino da cestino/drag and drop e si tratta di
              // cartella va aggiunto il watch
              if (event->mask & IN_ISDIR &&
                  cookie_map.find(event->cookie) == cookie_map.end()) {
                add_watch(full_path);
              }
              handlewatcher->handle_InRename(cookie_map[event->cookie],
                                             event->name);
              cookie_map.erase(event->cookie);
            }

            // L'evento in basso, capisce che è stato eliminato un file da GUI
            // if (event->mask & IN_MOVE)
            // handlewatcher->handle_InMove(full_path);

          }

          catch (const std::filesystem::filesystem_error &e) {

            std::clog << e.what() << "\n";
          }
        }
      }
    }
  }
};

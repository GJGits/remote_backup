#pragma once
#include "handle_watcher.hpp"
#include "sync_structure.hpp"
#include <chrono>
#include <condition_variable>
#include <errno.h>
#include <future> // std::async, std::future
#include <iostream>
#include <mutex>
#include <poll.h>
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
  std::thread scanner;
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
  void scan() {
    scanner = std::move(std::thread([&]() {
      HandleWatcher *watcher = HandleWatcher::getInstance();
      while (1) {
        // 1. elimina da file se un entry e' presente in structure.json, ma non
        //    nel filesystem
        watcher->handle_prune();
        // 2. se nel filesystem esiste qualcosa che non e' presente nella
        // structure
        //    aggiungo.
        watcher->handle_expand("./sync");
        // todo: sincronizzare con watcher, la sincronizzazione e' da valutare
        // nei
        //       singoli metodi di handle
        sleep(120);
      }
    }));
  }

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
    scanner.join();
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

          try {

            if (event->mask & IN_CREATE) {
              if (event->mask & IN_ISDIR) {
                add_watch(full_path);
                handlewatcher->handle_expand(full_path);
              }
            }

            if (event->mask & IN_CLOSE_WRITE ) {
                    handlewatcher->handle_InCloseWrite(full_path);
            }

            if (event->mask & IN_DELETE) {
              if (event->mask & IN_ISDIR) {
                remove_watch(full_path);
                handlewatcher->handle_prune();
              } else {
                  handlewatcher->handle_InDelete(full_path);
              }
            }

            if (event->mask & IN_MOVED_FROM) {
                handlewatcher->handle_prune();
              if (event->cookie != 0)
                cookie_map[event->cookie] = full_path;
            }

            if (event->mask & IN_MOVED_TO) {
                if (event->mask & IN_ISDIR &&
                    cookie_map.find(event->cookie) == cookie_map.end()) {
                    /* Quando viene effettuato il taglia e incolla, o premuto CTRL+Z */
                    add_watch(full_path);
                    handlewatcher->handle_expand(full_path);

                } else if(!(event->mask & IN_ISDIR)){
                    if (cookie_map.find(event->cookie) == cookie_map.end()) {

                        handlewatcher->handle_InCloseWrite(full_path);
                    } else {
                        handlewatcher->handle_InRename(full_path);
                        cookie_map.erase(event->cookie); // todo: Valutarne eventuale cancellazione di questa riga
                    }
                }
            }

          }

          catch (const std::filesystem::filesystem_error &e) {

            std::clog << e.what() << "\n";
          }
        }
      }
    }
  }
};

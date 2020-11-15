#pragma once

#include <bits/stdc++.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unordered_map>

#include "../modules/module.hpp"
#include "linux_event.hpp"

#define TIMER 10000
#define WAIT -1

class LinuxWatcher
    : public Singleton<LinuxWatcher>,
      public Module {
private:
  friend class Singleton;
  std::thread watcher;
  int pipe_[2];
  int timer;
  int inotify_descriptor;
  std::string root_to_watch;
  uint32_t watcher_mask;
  std::unordered_map<std::string, int> path_wd_map;
  std::unordered_map<int, std::string> wd_path_map;
  std::unordered_map<std::string, LinuxEvent> events;
  LinuxWatcher();

public:
  /**
   * Permette di aggiungere o modificare un watch all'inotify descriptor.
   *
   * @path: path relativo alla cartella o al file da monitorare.
   *
   * @throw: filesystem_error se root non presente
   */
  void add_watch(const std::string &path);

  /**
   * Dato il path di un file o di una cartella permette di rimuovere il
   * watching precedentemente creato.
   *
   * @path: path relativo al file o directory di cui si vuole smettere di
   *        ricevere notifiche.
   * @return: true in caso di successo, false altrimenti.
   */
  bool remove_watch(const std::string &path);

  /**
   * Effettua watch degli eventi on-line per poi effettuare una publish
   * al broker.
   */
  void handle_events();

  /**
   * Metodo che permette di chiamare le subscribe ai topic ai quali questo
   * modulo è interessato. Attualmente questo modulo non prevede ulteriori
   * sottoscrizioni.
   */
  void init_sub_list();

  /**
   * Overload di Module::start(), necessario per definire la politica di
   * start del modulo specifico
   */
  void start();

  /**
   * Overload di Module::stop(), necessario per definire la politica di
   * stop del modulo specifico
   */
  void stop();

  ~LinuxWatcher();
};
#pragma once

#include <bits/stdc++.h>
#include <chrono>
#include <errno.h>
#include <iostream>
#include <memory>
#include <poll.h>
#include <regex>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/inotify.h>
#include <unistd.h>
#include <unordered_map>

#include "../common/duration.hpp"
#include "../common/json.hpp"
#include "../common/singleton.hpp"
#include "../pubsub/broker.hpp"
#include "../pubsub/message.hpp"
#include "sync_structure.hpp"
#include <stdio.h>
#include <unistd.h>

#define TIMER 5000
#define WAIT -1

using json = nlohmann::json;

class LinuxWatcher
    : public ParamSingleton<LinuxWatcher, std::string, u_int32_t> {
private:
  friend class ParamSingleton;
  int pipe_[2];
  int timer;
  int inotify_descriptor;
  std::string root_to_watch;
  uint32_t watcher_mask;
  bool running;
  std::regex temp_rgx;
  std::regex bin_rgx;
  std::set<std::string> new_files;
  std::unordered_map<std::string, int> path_wd_map;
  std::unordered_map<int, std::string> wd_path_map;
  std::unordered_map<int, std::string> cookie_map;
  LinuxWatcher(const std::string &root_to_watch, uint32_t mask);

public:
  /**
   * Permette di aggiungere o modificare un watch all'inotify descriptor.
   *
   * @path: path relativo alla cartella o al file da monitorare.
   * @mask: 4 byte per descrivere il codice di evento da monitorare
   *
   * @return: true se l'evento e' stato aggiunto alla lista del monitoring,
   *          false altrimenti.
   */
  bool add_watch(const std::string &path);

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
   * modulo è interessato.
   */
  void init_sub_list();

  /**
   * Esegue chiamate necessarie in fase di inizializzazione del modulo.
   */
  void start(const Message &message);

  /**
   * Esegue chiamate necessarie in fase di stop del modulo.
   */
  void stop(const Message &message);

  ~LinuxWatcher() {
    std::clog << "Linux Watcher destroy...\n";
    // Quando l'oggetto viene distrutto rilascio il file descriptor
    // in questo modo il kernel ha la possibilità di riassegnarlo ad
    // un altro processo.
    close(inotify_descriptor);
  }
};
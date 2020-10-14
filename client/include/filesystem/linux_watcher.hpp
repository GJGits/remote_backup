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
#include "../pubsub/broker.hpp"
#include "../pubsub/message.hpp"
#include "sync_structure.hpp"

#define TIMER 5000
#define WAIT -1

using json = nlohmann::json;

class LinuxWatcher {
private:
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
  static inline std::shared_ptr<LinuxWatcher> instance{nullptr};

  LinuxWatcher(const std::string &root_to_watch, uint32_t mask);

public:
  static std::shared_ptr<LinuxWatcher> getInstance(const std::string &root_path,
                                                   uint32_t mask);
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
   * Metodo eseguito a startup e che permette di notificare l'aggiunta
   * di nuovi o modifica di file avvenuta off-line. Il controllo che viene
   * effettuato per la modifica si basa su last_mod.
   */
  void check_news();

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
    // Quando l'oggetto viene distrutto rilascio il file descriptor
    // in questo modo il kernel ha la possibilità di riassegnarlo ad
    // un altro processo.
    close(inotify_descriptor);
  }
};
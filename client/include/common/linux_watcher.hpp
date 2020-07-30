#include "unistd.h"
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
#include "handle_watcher.hpp"

/**
 * Classe singleton che racchiude un metodo statico per ogni evento da
 * monitorare
 **/
class LinuxWatcher {
private:
  int inotify_descriptor;
  std::unordered_map<std::string, int> path_wd_map;
  std::unordered_map<int, std::string> wd_path_map;
  std::string root_to_watch;
  inline static LinuxWatcher *instance = nullptr;

  LinuxWatcher(const std::string &root_path, uint32_t mask) {
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
        // Aggiungiamo una root to watch, senza questa il watcher non ha
        // senso di esistere.
        root_to_watch = root_path;
        add_watch(root_path, mask);
    }

public:

    static LinuxWatcher *getInstance(const std::string &root_path, uint32_t mask) {
        if (instance == nullptr) {
            instance = new LinuxWatcher(root_path,mask);
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
  bool add_watch(const std::string &path, uint32_t mask) {
    int wd = inotify_add_watch(inotify_descriptor, path.c_str(), mask);
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
      memset(buf,'\0',4096);
      const struct inotify_event *event;
      ssize_t len;
      char *ptr; // ptr per consumare il buffer
      len = read(inotify_descriptor, buf, sizeof buf);
      // todo: check su read qui...

      for (ptr = buf; ptr < buf + len;
           ptr += sizeof(struct inotify_event) + event->len) {

        event = (const struct inotify_event *)ptr;
        //std::clog << "path: " << wd_path_map[event->wd] << "\n";

        Handle_watcher *handlewatcher=Handle_watcher::getInstance();

        // Print event type
        /*  if (event->mask & IN_ATTRIB)
              std::clog << "IN_ATTRIB: (" << event->name << ")\n";
          if (event->mask & IN_OPEN)
              std::clog << "IN_OPEN: (" << event->name << ")\n";
          if (event->mask & IN_CLOSE_NOWRITE)
              std::clog << "IN_CLOSE_NOWRITE: (" << event->name << ")\n";
          if (event->mask & IN_CLOSE_WRITE)
              std::clog << "IN_CLOSE_WRITE: (" << event->name << ")\n";
        */

        /*if (event->mask & IN_OPEN)
             handlewatcher.handle_InOpen(event->name);

        if (event->mask & IN_CLOSE_NOWRITE)
            handlewatcher.handle_InCloseNoWrite(event->name);

        if (event->mask & IN_CLOSE_WRITE)
            handlewatcher.handle_InCloseWrite(event->name);*/

        if (event->mask & IN_CREATE)
            handlewatcher->handle_InCreate(event->name);

        // la Delete funziona solo da terminale e non capisco perchè
        if (event->mask & IN_DELETE)
            handlewatcher->handle_InDelete(event->name);

        if (event->mask & IN_MODIFY)
            handlewatcher->handle_InModify(event->name);

        if(event->mask & IN_MOVED_TO)
            handlewatcher->handle_InRename(event->name);



        // print path su cui e' avvenuto l'evento
        std::clog << "path: " << wd_path_map[event->wd];
      }
    }
  }
};
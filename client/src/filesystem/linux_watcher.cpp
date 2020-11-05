
#include "../../include/filesystem/linux_watcher.hpp"

LinuxWatcher::LinuxWatcher()
    : root_to_watch{"./sync"}, watcher_mask{IN_CREATE | IN_ONLYDIR | IN_DELETE |
                                            IN_MODIFY | IN_MOVED_TO |
                                            IN_MOVED_FROM | IN_ISDIR |
                                            IN_IGNORED},
      running{false} {
  std::clog << "Linux watcher module init...\n";
  // pipe per segnale exit al poll.
  //  - watcher scrive su 1
  //  - poll scrive su 0
  pipe(pipe_);
  timer = TIMER;
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

LinuxWatcher::~LinuxWatcher() {
  std::clog << "Linux Watcher destroy...\n";
  // Quando l'oggetto viene distrutto rilascio il file descriptor
  // in questo modo il kernel ha la possibilità di riassegnarlo ad
  // un altro processo.
  close(inotify_descriptor);
}

void LinuxWatcher::start() {
  std::clog << "Linux watcher module start...\n";
  running = true;
  if (!std::filesystem::exists(root_to_watch))
    throw SyncNotValid();
  instance->add_watch(root_to_watch);
  instance->handle_events();
}

void LinuxWatcher::stop() {
  running = false;
  instance->remove_watch(root_to_watch);
  short poll_sig = 1;
  write(pipe_[1], &poll_sig, 1);
  watcher.join();
  std::clog << "Linux watcher module stop...\n";
}

bool LinuxWatcher::add_watch(const std::string &path) {
  int wd = inotify_add_watch(inotify_descriptor, path.c_str(), watcher_mask);
  if (wd == -1) {
    return false;
  }
  path_wd_map[path] = wd;
  wd_path_map[wd] = path;
  for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
    if (p.is_directory()) {
      add_watch(p.path().string());
    }
  }
  return true;
}

bool LinuxWatcher::remove_watch(const std::string &path) {
  if (path_wd_map.find(path) == path_wd_map.end() ||
      path.compare(root_to_watch) == 0)
    return false;
  inotify_rm_watch(inotify_descriptor, path_wd_map[path]);
  wd_path_map.erase(path_wd_map[path]);
  path_wd_map.erase(path);
  return true;
}

void LinuxWatcher::handle_events() {

  watcher = std::move(std::thread{[&]() {
    std::clog << "Start monitoring... [thread " << std::this_thread::get_id()
              << "]\n";
    std::string tmp_path{"./sync/.tmp"};
    std::string bin_path{"./sync/.bin"};
    while (running) {
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
      nfds_t nfds = 2;
      struct pollfd fds[2];
      fds[0].fd = inotify_descriptor;
      fds[0].events = POLLIN; // maschera generale per eventi su fd?
      fds[1].fd = pipe_[0];
      fds[1].events = POLLIN;

      // poll until an event occurs.Timeout = -1 -> BLOCKING,
      // else timeout expressed in milliseconds
      poll_num = poll(fds, nfds, timer);
      if (fds[1].revents & POLLIN) {
        break;
      }

      if (poll_num > 0) {
        len = read(inotify_descriptor, buf, sizeof buf);
        // todo: check su read qui...

        for (ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len) {

          DurationLogger duration{"HANDLING_EVENT"};

          event = (const struct inotify_event *)ptr;

          const std::string path =
              wd_path_map[event->wd] + "/" + std::string{event->name};

          // struct inotify_event evento{0, 256, 0, 0};
          // strcpy(evento.name, path.c_str());
          // std::clog << "evento nome: " << evento.name << "\n";

          timer = TIMER;

          // eventi non presenti in inotify:
          // 1073742080 -> copia incolla cartella da gui da fuori sync (con file
          // e sotto cartelle) 1073741888 -> eliminazione cartella da gui (con
          // file e sotto cartelle) 1073742336 -> eliminazione cartella da
          // command line 1073741952 -> move da linea di comando riguardante una
          // cartella 1073741888 -> move from cartella da terminale 1073741952
          // -> move to cartella da terminale

          switch (event->mask) {

          case 1073742080:
          case 1073741952: {
            add_watch(path);
            for (auto &p :
                 std::filesystem::recursive_directory_iterator(path)) {
              if (p.is_regular_file()) {
                if (!(path.rfind(tmp_path, 0) == 0)) {
                  std::string f_path = p.path().string();
                  LinuxEvent ev{f_path, 0, 256};
                  events[f_path] = ev;
                }
              }
            }

          } break;

          case 1073741888:
          case 1073742336: {
            std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
            for (std::string &sync_path : sync->get_paths()) {
              if (!std::filesystem::exists(sync_path)) {
                LinuxEvent ev{sync_path, 0, 512};
                events[sync_path] = ev;
              }
            }

          } break;

            // to tmp -> non loggare ok
            // from tmp -> non loggare, ma conserva cookie ok
            // to sync -> loggo se from non da tmp
            // rm tmp -> non loggare ok
            // from sync -> loggo se to non bin

          default: {

            uint32_t mask = event->mask;
            if (((path.rfind(tmp_path, 0) == 0) && mask == 64) ||
                ((path.rfind(bin_path, 0) == 0) && mask == 128) ||
                (!(path.rfind(tmp_path, 0) == 0) &&
                 !(path.rfind(bin_path, 0) == 0) &&
                 (mask == 2 || mask == 64 || mask == 128 || mask == 256 ||
                  mask == 512))) {
              LinuxEvent ev{path, event->cookie, event->mask};
              events[path] = ev;
            }

          } break;
          }
        }
      }
      if (poll_num == 0) {

        timer = WAIT;

        std::vector<LinuxEvent> eves{};
        for (const auto &[path, event] : events) {
          eves.push_back(event);
        }

        // ordine descrescente (cookie, mask)
        std::sort(eves.begin(), eves.end(),
                  [&](const LinuxEvent &ev1, const LinuxEvent &ev2) {
                    if (ev1.get_cookie() != ev2.get_cookie())
                      return ev1.get_cookie() > ev2.get_cookie();
                    return ev1.get_mask() > ev2.get_mask();
                  });

        for (auto it = eves.begin(); it != eves.end(); it++) {
          std::string path = it->get_path();
          uint32_t mask = it->get_mask();
          // 1. se to sync e from tmp or to bin from sync -> skippo
          if ((!(path.rfind(tmp_path, 0) == 0) &&
               !(path.rfind(bin_path, 0) == 0) &&
               (mask == 128 && (it + 1) != eves.end() &&
                (it + 1)->get_mask() == 64 &&
                (it + 1)->get_path().rfind(tmp_path, 0) == 0)) ||
              (mask == 128 && path.rfind(bin_path, 0) == 0)) {
            it++;
            continue;
          }
          // 2. altro -> invio messaggio
          if (mask == 2 || mask == 128 || mask == 256) {
            std::shared_ptr<FileEntry> content{
                new FileEntry{path, entry_producer::local, entry_status::new_}};
            broker->publish(Message{TOPIC::NEW_FILE, content});
          }
          if (mask == 64 || mask == 512) {
            std::shared_ptr<FileEntry> content{new FileEntry{
                path, entry_producer::local, entry_status::delete_}};
            broker->publish(Message{TOPIC::FILE_DELETED, content});
          }
        }

        events.clear();
      }
    }
  }});
}
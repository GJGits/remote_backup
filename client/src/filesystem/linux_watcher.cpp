#include "../../include/filesystem/linux_watcher.hpp"

LinuxWatcher::LinuxWatcher(const std::string &root_to_watch, uint32_t mask)
    : root_to_watch{root_to_watch}, watcher_mask{mask}, running{false} {
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
  // regex che serve per evitare eventi su e da cartella .tmp
  // cartella utilizzata per poggiare i download
  temp_rgx = std::move(std::regex{"\\.\\/sync\\/\\.tmp\\/.*"});
  // regex che serve per evitare eventi su e da cartella .bin
  // cartella utilizzata per poggiare i file da eliminare
  bin_rgx = std::move(std::regex{"\\.\\/sync\\/\\.bin\\/.*"});
}

void LinuxWatcher::init_sub_list() {
  std::shared_ptr<Broker> broker = Broker::getInstance();
  broker->subscribe(
      TOPIC::LOGGED_IN,
      std::bind(&LinuxWatcher::start, instance.get(), std::placeholders::_1));
  broker->subscribe(
      TOPIC::LOGGED_OUT,
      std::bind(&LinuxWatcher::stop, instance.get(), std::placeholders::_1));
  broker->subscribe(TOPIC::CLOSE, std::bind(&LinuxWatcher::stop, instance.get(),
                                            std::placeholders::_1));
}

void LinuxWatcher::start(const Message &message) {
  std::clog << "Linux watcher module start...\n";
  running = true;
  instance->add_watch(root_to_watch);
  instance->handle_events();
}

void LinuxWatcher::stop(const Message &message) {
  running = false;
  instance->remove_watch(root_to_watch);
  short poll_sig = 1;
  write(pipe_[1], &poll_sig, 1);
  std::clog << "Linux watcher module exit...\n";
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

  std::clog << "Start monitoring...\n";
  std::shared_ptr<Broker> broker = Broker::getInstance();
  while (running) {
    std::clog << "Wating for an event...\n";
    // Some systems cannot read integer variables if they are not
    // properly aligned. On other systems, incorrect alignment may
    // decrease performance. Hence, the buffer used for reading from
    // the inotify file descriptor should have the same alignment as
    // struct inotify_event.

    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
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

        json message;
        const std::string path =
            wd_path_map[event->wd] + "/" + std::string{event->name};
        message["path"] = path;

        timer = TIMER;
        std::clog << "watch path: " << wd_path_map[event->wd] << "\n";
        std::clog << "MASK: " << event->mask << " NAME " << event->name << "\n";

        // eventi non presenti in inotify:
        // 1073742080 -> copia incolla cartella da gui da fuori sync (con file
        // e sotto cartelle) 1073741888 -> eliminazione cartella da gui (con
        // file e sotto cartelle) 1073742336 -> eliminazione cartella da
        // command line 1073741952 -> move da linea di comando riguardante una
        // cartella 1073741888 -> move from cartella da terminale 1073741952
        // -> move to cartella da terminale

        std::smatch match;

        if (!std::regex_match(path, match, temp_rgx) &&
            !std::regex_match(path, match, bin_rgx)) {

          switch (event->mask) {

          case 1073742080:
          case 1073741952: {
            add_watch(path);
            for (auto &p :
                 std::filesystem::recursive_directory_iterator(path)) {
              if (p.is_regular_file()) {
                std::string f_path = p.path().string();
                LinuxEvent ev{f_path, 0, 256};
                events[f_path] = ev;
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

          default: {
            LinuxEvent ev{path, event->cookie, event->mask};
            events[path] = ev;

          } break;
          }
        }
      }
    }
    if (poll_num == 0) {

      timer = WAIT;
      std::vector<LinuxEvent> eventi{};

      for (const auto &[key, value] : events) {
        eventi.push_back(value);
      }

      std::sort(eventi.begin(), eventi.end(),
                [&](const LinuxEvent &eve1, const LinuxEvent &eve2) {
                  if (eve1.get_cookie() != eve2.get_cookie()) {
                    return eve1.get_cookie() > eve2.get_cookie();
                  }
                  return eve1.get_mask() > eve2.get_mask();
                });

      for (auto it = eventi.begin(); it != eventi.end(); ++it) {

        // moved_to con from -> rename o new_file
        if (it->get_mask() == 128 && (it + 1) != eventi.end() &&
            (it + 1)->get_mask() == 64 &&
            it->get_cookie() == (it + 1)->get_cookie()) {
          std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
          if (sync->has_path((it + 1)->get_path())) {
            // rename
            json mex = {{"old_path", (it + 1)->get_path()},
                        {"new_path", it->get_path()}};
            broker->publish(Message{TOPIC::FILE_RENAMED, mex});
          } else {
            // new file
            json mex = {{"path", it->get_path()}};
            broker->publish(Message{TOPIC::NEW_FILE, mex});
          }
          ++it;
          continue;
        }

        if (it->get_mask() == 2 || it->get_mask() == 256 ||
            (it->get_mask() == 128 && (it + 1) != eventi.end() &&
             (it + 1)->get_mask() != 64) ||
            (it->get_mask() == 128 && (it + 1) == eventi.end())) {
          json mex = {{"path", it->get_path()}};
          broker->publish(Message{TOPIC::NEW_FILE, mex});
        }

        // file moved or deleted
        if (it->get_mask() == 64 || it->get_mask() == 512) {
          // delete file
          json mex = {{"path", it->get_path()}};
          broker->publish(Message{TOPIC::FILE_DELETED, mex});
        }
      }

      events.clear();
    }
  }
}

#include "../../include/filesystem/linux_watcher.hpp"

LinuxWatcher::LinuxWatcher(const std::string &root_to_watch, uint32_t mask)
    : root_to_watch{root_to_watch}, watcher_mask{mask}, running{false} {
  // pipe per segnale exit al poll.
  //  - watcher scrive su 1, legge su 0
  //  - poll scrive su 0, legge su 1.
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
  std::clog << "watcher started...\n";
  running = true;
  instance->add_watch(root_to_watch);
  instance->check_news();
  instance->handle_events();
}

void LinuxWatcher::stop(const Message &message) {
  running = false;
  instance->remove_watch(root_to_watch);
  write(pipe_[1], "a", 1);
  std::clog << "watcher exit...\n";
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

void LinuxWatcher::check_news() {
  std::shared_ptr<SyncStructure> sync_structure = SyncStructure::getInstance();
  std::shared_ptr<Broker> broker = Broker::getInstance();
  json message;
  for (auto &p : std::filesystem::recursive_directory_iterator(root_to_watch)) {
    std::string sub_path = p.path().string();
    if (std::filesystem::is_regular_file(sub_path)) {
      // size_t last_mod = std::filesystem::last_write_time(sub_path)
      //                       .time_since_epoch()
      //                       .count() /
      //                   1000000000;
      message["path"] = sub_path;
      // 1. file non presente in struttura -> aggiunto off-line
      if (!sync_structure->has_entry(sub_path)) {
        broker->publish(Message{TOPIC::NEW_FILE, message});
      }
      // 2. last_mod non coincide -> modificato off-line
      // else if (sync_structure->get_last_mod(sub_path) < last_mod) {
      // broker->publish(Message{TOPIC::FILE_MODIFIED, message});
      //}
    }
  }
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
    if (!running)
      break;
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

        try {
          timer = TIMER;
          std::clog << "watch path: " << wd_path_map[event->wd] << "\n";
          std::clog << "MASK: " << event->mask << "NAME" << event->name << "\n";

          //
          //  TABELLA RIASSUNTIVA CODICI EVENTI:

          //    8          -> EVENTO IN_CLOSE_WRITE
          //    256        -> FILE WAS CREATED
          //    1073741952 -> CTRL+Z CARTELLA ?
          //    1073742080 -> EVENT HANDLE_EXPAND
          //    1073741888 -> MOVED_FROM FOLDER
          //    64         -> MOVED_FROM FILE
          //    128        -> RENAME  (MOVED_TO)
          //    512        -> IN_DELETE

          switch (event->mask) {
          case 2: {
            // Un file relativamente grande genera in seguito ad una NEW_FILE
            // una serie di FILE_MODIFIED. In questo modo lascio fare tutto
            // alla NEW_FILE
            if (new_files.find(path) == new_files.end()) {
              std::smatch match;
              std::smatch match2;
              if (!std::regex_search(path.begin(), path.end(), match,
                                     temp_rgx) &&
                  !std::regex_search(path.begin(), path.end(), match2, bin_rgx))
                broker->publish(Message{TOPIC::FILE_MODIFIED, message});
            }

          } break;
          case 256: {
            new_files.insert(path);
            std::smatch match1;
            std::smatch match2;
            if (!std::regex_search(path.begin(), path.end(), match1,
                                   temp_rgx) &&
                !std::regex_search(path.begin(), path.end(), match2, bin_rgx))
              broker->publish(Message{TOPIC::NEW_FILE, message});
          }

          break;
          case 1073742080:
            add_watch(message["path"]);
            broker->publish(Message{TOPIC::NEW_FOLDER, message});
            break;
          case 1073741952:
            add_watch(message["path"]);
            broker->publish(Message{TOPIC::NEW_FOLDER, message});
            break;
          case 1073741888:
            remove_watch(message["path"]);
            // broker->publish(TOPIC::CONTENT_MOVED, Message{});
            break;
          case 64:
            cookie_map[event->cookie] = message["path"];
            // broker->publish(TOPIC::CONTENT_MOVED, Message{});
            break;
          case 128: {
            if (cookie_map.find(event->cookie) != cookie_map.end()) {
              std::smatch match1;
              std::smatch match2;
              if (!std::regex_search(path.begin(), path.end(), match1,
                                     temp_rgx) &&
                  !std::regex_search(path.begin(), path.end(), match2,
                                     bin_rgx)) {
                message["old_path"] = cookie_map[event->cookie];
                broker->publish(Message{TOPIC::FILE_RENAMED, message});
                cookie_map.erase(event->cookie);
              }
            }
          } break;
          case 512: {
            std::smatch match;
            if (!std::regex_search(path.begin(), path.end(), match, bin_rgx))
              broker->publish(Message{TOPIC::FILE_DELETED, message});
          }

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
      new_files.clear();
      cookie_map.clear();
      timer = WAIT;
      // check di move verso l'esterno
      std::shared_ptr<Broker> broker = Broker::getInstance();
      std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
      json mex;
      for (std::string &path : sync->get_paths()) {
        if (!std::filesystem::exists(path)) {
          mex["path"] = path;
          broker->publish(Message{TOPIC::FILE_DELETED, mex});
        }
      }
    }
  }
}

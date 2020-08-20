#pragma once

#include "../common/json.hpp"
#include "event.hpp"
#include "file_entry.hpp"
#include "message.hpp"
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <queue>
#include <regex>
#include <thread>
#include <vector>

using json = nlohmann::json;

class HandleWatcher {
  inline static HandleWatcher *instance = nullptr;
  int rename_counter;
  std::queue<Event> events;
  std::queue<Message> messages;
  std::thread dispatcher;
  std::mutex m, mex_m;
  std::condition_variable cv, mex_cv;
  bool dirty_file;
  bool finish;
  bool to_clean;
  bool startup;
  void run_dispatcher();
  void handle_InCloseWrite(const std::string &);
  void handle_InModify(const std::string &);
  void handle_InDelete(const std::string &);
  void handle_prune();
  void handle_expand(const std::string &);
  void handle_InRename(const std::string &old_path,
                       const std::string &new_path);
  void handle_InMove(const std::string &);

public:
  static HandleWatcher *getInstance();
  ~HandleWatcher();
  void push_event(const Event &event);
  void push_message(const Message &message);
  Message pop_message(bool structure);
  void increase_counter() { rename_counter++; }
  void decrease_counter() { rename_counter--; };
  int get_count() const { return rename_counter; }
  void reset_counter() { rename_counter = 0; }
  void set_dirty(bool flag) { dirty_file = flag; }
  bool is_dirty() const { return dirty_file;}
};

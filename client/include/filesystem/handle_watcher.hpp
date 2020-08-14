#pragma once

#include "../common/json.hpp"
#include "event.hpp"
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
  std::queue<Event> events;
  std::vector<std::thread> workers;
  std::mutex m;
  std::mutex real_clean_mutex;
  std::condition_variable cv;
  bool finish;
  bool to_clean;
  void run_workers();
  void handle_InCloseWrite(const std::string &);
  void handle_InModify(const std::string &);
  void handle_InDelete(const std::string &);
  void handle_prune();
  void handle_expand(const std::string &);
  void handle_InRename(const std::string &old_path, const std::string &new_path);
  void handle_InMove(const std::string &);

public:
  static HandleWatcher *getInstance();
  ~HandleWatcher();
  void push_event(const Event &event);
  void set_toclean(){ to_clean = true; };

};

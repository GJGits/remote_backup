#include "../../include/filesystem/resource_guard.hpp"
#include <iostream>

resource_guard::resource_guard(/* args */) {}

resource_guard::~resource_guard() {
  if (!std::filesystem::exists(SYNC_ROOT)) {
    std::filesystem::create_directories(SYNC_ROOT);
    reset_struct();
  }
  std::filesystem::create_directory(TMP_PATH);
  std::filesystem::create_directory(BIN_PATH);

  if (!std::filesystem::exists(CLIENT_STRUCT)) {
    std::clog << "RESETTO_NON_ESISTE\n";
  }

  if (std::filesystem::is_empty(CLIENT_STRUCT)) {
    std::clog << "RESETTO_EMPTY\n";
  }

  if (!std::filesystem::exists(CLIENT_STRUCT) ||
      std::filesystem::is_empty(CLIENT_STRUCT)) {
    std::clog << "RESETTO\n";
    reset_struct();
  }
}

void resource_guard::reset_struct() {
  std::ofstream o(CLIENT_STRUCT);
  json struct_ = {{"entries", json::array()}, {"last_check", 0}};
  o << struct_ << "\n";
}
#include "../../include/filesystem/resource_guard.hpp"

resource_guard::resource_guard(/* args */) {}

resource_guard::~resource_guard() {
  if (!std::filesystem::exists("./sync")) {
    std::filesystem::create_directories("./sync");
    reset_struct();
  }
  std::filesystem::create_directory("./sync/.tmp");
  std::filesystem::create_directory("./sync/.bin");
  if (!std::filesystem::exists("./config/client-struct.json")) {
    reset_struct();
  }
}

void resource_guard::reset_struct() {
  std::ofstream o("./config/client-struct.json");
  json struct_ = {{"entries", json::array()}, {"last_check", 0}};
  o << struct_ << "\n";
}
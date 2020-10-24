#pragma once

#include <string>

class LinuxEvent {
private:
  std::string path;
  uint32_t cookie;
  uint32_t mask;

public:
  LinuxEvent() {}
  LinuxEvent(const std::string &path, uint32_t cookie, uint32_t mask)
      : path{path}, cookie{cookie}, mask{mask} {}
  std::string get_path() const { return path; }
  uint32_t get_cookie() const { return cookie; }
  uint32_t get_mask() const { return mask; }
};
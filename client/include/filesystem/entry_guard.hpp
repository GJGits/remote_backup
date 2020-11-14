#pragma once

#include "file_entry.hpp"

class entry_guard {
private:
  std::shared_ptr<FileEntry> entry;

public:
  entry_guard(const std::shared_ptr<FileEntry> &fentry);
  ~entry_guard();
};

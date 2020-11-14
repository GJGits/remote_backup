#include "../../include/filesystem/entry_guard.hpp"

entry_guard::entry_guard(const std::shared_ptr<FileEntry> &fentry)
    : entry{fentry} {}

entry_guard::~entry_guard() {
  if (!std::filesystem::exists(entry->get_path())) {
    entry->set_nchunks(0);
    entry->set_read_count(0);
    entry->set_status(entry_status::delete_);
  }
}
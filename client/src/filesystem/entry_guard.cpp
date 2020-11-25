#include "../../include/filesystem/entry_guard.hpp"

entry_guard::entry_guard(const std::shared_ptr<FileEntry> &fentry)
    : entry{fentry} {}

entry_guard::~entry_guard() {
  if (!std::filesystem::exists(entry->get_path())) {
    entry->set_nchunks(0);
    entry->set_read_count(0);
    if (entry->get_producer() == entry_producer::local &&
        entry->get_status() == entry_status::new_) {
      entry->set_status(entry_status::delete_);
    }
  }
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->add_entry(entry);
}

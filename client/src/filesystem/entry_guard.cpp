#include "../../include/filesystem/entry_guard.hpp"

entry_guard::entry_guard(const std::shared_ptr<FileEntry> &fentry)
    : entry{fentry} {}

entry_guard::~entry_guard() {
  if (!std::filesystem::exists(entry->get_path())) {
    entry->set_nchunks(0);
    entry->set_read_count(0);
    // se in temp ho un file .out e la entry ha come status new
    // allora ho un download non completato a causa di una qualche
    // eccezione. In questo caso elimino il file in .tmp per non 
    // accumulare
    if (std::filesystem::exists(TMP_PATH + std::string{"/"} +
                                macaron::Base64::Encode(entry->get_path()) +
                                std::string{".out"}) && entry->get_status() == entry_status::new_) {
      std::filesystem::remove(TMP_PATH + std::string{"/"} +
                              macaron::Base64::Encode(entry->get_path()) +
                              std::string{".out"});
    }

    if (entry->get_producer() == entry_producer::local &&
        entry->get_status() == entry_status::new_) {
      entry->set_status(entry_status::delete_);
    }
  }
}

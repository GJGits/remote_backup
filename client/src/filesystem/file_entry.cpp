#include "../../include/filesystem/file_entry.hpp"

FileEntry::FileEntry(const std::string &path) : path{path} {
  entry["path"] = path;
  entry["last_mod"] =
      std::filesystem::last_write_time(path).time_since_epoch().count() /
      1000000000;
  size = std::filesystem::file_size(path);
  entry["size"] = size;
  nchunks = ceil((double)size / CHUNK_SIZE);
}

uintmax_t FileEntry::get_size() { return size; }
void FileEntry::add_chunk(json &chunk) { entry["chunks"].push_back(chunk); }
size_t FileEntry::get_nchunks() { return nchunks; }
void FileEntry::clear_chunks() { entry["chunks"].clear(); }
json FileEntry::get_json_representation() { return entry; }
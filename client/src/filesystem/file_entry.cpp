#include "../../include/filesystem/file_entry.hpp"

FileEntry::FileEntry(const std::string &path) : path{path} {
  read_count = 0;
  entry["path"] = path;
  entry["last_mod"] =
      std::filesystem::last_write_time(path).time_since_epoch().count() /
      1000000000;
  size = std::filesystem::file_size(path);
  entry["size"] = size;
  nchunks = ceil((double)size / CHUNK_SIZE);
  entry["nchunks"] = nchunks;
  buffer = std::shared_ptr<char[]>{new char[CHUNK_SIZE]};
  memset(buffer.get(), '\0', CHUNK_SIZE);
}

bool FileEntry::has_chunk() { return read_count < nchunks; }

std::tuple<std::shared_ptr<char[]>, size_t> FileEntry::next_chunk() {
  if (!in.is_open()) {
    in = std::move(std::ifstream{path, std::ios::binary});
  }
  memset(buffer.get(), '\0', CHUNK_SIZE);
  size_t to_read =
      read_count < (nchunks - 1) ? CHUNK_SIZE : (size - ((nchunks - 1) * CHUNK_SIZE));
  in.seekg(read_count * CHUNK_SIZE);
  in.read(buffer.get(), to_read);
  json chunk = {{"id", read_count},
                {"hash", Sha256::getSha256(buffer, to_read)}};
  entry["chunks"].push_back(chunk);
  read_count++;
  return std::tuple(buffer, to_read);
}

size_t FileEntry::get_nchunks() { return nchunks; }
void FileEntry::clear_chunks() { entry["chunks"].clear(); }
json FileEntry::get_json_representation() { return entry; }
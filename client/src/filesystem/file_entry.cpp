#include "../../include/filesystem/file_entry.hpp"

FileEntry::FileEntry(const std::string &path) : path{path} {
  read_count = 0;
  entry["path"] = path;
  struct stat sb;
  stat(path.c_str(), &sb);
  size = std::filesystem::file_size(path);
  nchunks = ceil((double)size / CHUNK_SIZE);
  entry["transfers"]["nchunks"] = nchunks;
  entry["transfers"]["direction"] = std::string{"up"};
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
  entry["transfers"]["chunks"].push_back(read_count);
  read_count++;
  return std::tuple(buffer, to_read);
}

size_t FileEntry::get_nchunks() { return nchunks; }
void FileEntry::clear_chunks() { entry["transfers"]["chunks"].clear(); }
json FileEntry::get_json_representation() { return entry; }
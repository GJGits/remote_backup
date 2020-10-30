#include "../../include/filesystem/file_entry.hpp"

FileEntry::FileEntry(const std::string &path) : path{path} {
  read_count = 0;
  entry["path"] = path;
  struct stat sb;
  stat(path.c_str(), &sb);
  int last_change = sb.st_ctime;
  entry["last_local_change"] = last_change;
  size = std::filesystem::file_size(path);
  nchunks = ceil((double)size / CHUNK_SIZE);
  entry["transfers"]["nchunks"] = nchunks;
  entry["transfers"]["direction"] = std::string{"up"};
  buffer = std::shared_ptr<char[]>{new char[CHUNK_SIZE]};
  memset(buffer.get(), '\0', CHUNK_SIZE);
}

bool FileEntry::has_chunk() { return read_count < nchunks; }

std::tuple<std::shared_ptr<char[]>, size_t> FileEntry::next_chunk() {
  DurationLogger log{"READ_CHUNK"};
  std::ifstream in{path, std::ios::binary};
  in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  size_t to_read = read_count < (nchunks - 1)
                       ? CHUNK_SIZE
                       : (size - ((nchunks - 1) * CHUNK_SIZE));
  in.seekg(read_count * CHUNK_SIZE);
  in.read(buffer.get(), to_read);
  json chunk = {{"id", read_count},
                {"hash", Sha256::getSha256(buffer, to_read)}};
  entry["transfers"]["chunks"].push_back(chunk);
  read_count++;
  return std::tuple(buffer, to_read);
}

size_t FileEntry::get_nchunks() { return nchunks; }
void FileEntry::clear_chunks() { entry["transfers"]["chunks"].clear(); }
json FileEntry::get_json_representation() { return entry; }
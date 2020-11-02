#include "../../include/filesystem/file_entry.hpp"

FileEntry::FileEntry() : nchunks{1}, read_count{0}, buffer{nullptr} {}

FileEntry::FileEntry(const std::string &path, entry_producer producer,
                     entry_status status)
    : path{path}, producer{producer}, nchunks{1}, status{status},
      read_count{0}, buffer{nullptr} {}

FileEntry::FileEntry(const std::string &path, entry_producer producer,
                     size_t nchunks, size_t last_change, entry_status status)
    : path{path}, producer{producer}, nchunks{nchunks},
      last_change{last_change}, status{status}, read_count{0}, buffer{nullptr} {
}

bool FileEntry::has_chunk() { return read_count < nchunks; }

std::tuple<std::shared_ptr<char[]>, size_t> FileEntry::next_chunk() {
  DurationLogger log{"READ_CHUNK"};
  std::ifstream in{path, std::ios::binary};
  in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  // lazy init of infos
  if (buffer.get() == nullptr) {
    size = std::filesystem::file_size(path);
    struct stat sb;
    stat(path.c_str(), &sb);
    last_change = (size_t)sb.st_ctime;
    buffer = std::shared_ptr<char[]>{new char[CHUNK_SIZE]};
    nchunks = ceil((double)std::filesystem::file_size(path) / CHUNK_SIZE);
    memset(buffer.get(), '\0', CHUNK_SIZE);
  }
  size_t to_read = read_count < (nchunks - 1)
                       ? CHUNK_SIZE
                       : (size - ((nchunks - 1) * CHUNK_SIZE));
  in.seekg(read_count * CHUNK_SIZE);
  in.read(buffer.get(), to_read);
  last_move = std::tuple{read_count, Sha256::getSha256(buffer, to_read)};
  return std::tuple(buffer, to_read);
}

size_t FileEntry::get_nchunks() const { return nchunks; }
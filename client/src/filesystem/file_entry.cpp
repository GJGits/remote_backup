#include "../../include/filesystem/file_entry.hpp"

FileEntry::FileEntry() : nchunks{0}, read_count{0}, buffer{nullptr} {}

FileEntry::FileEntry(const std::string &path, entry_producer producer,
                     entry_status status)
    : path{path}, producer{producer}, nchunks{0},
      last_change{0}, status{status}, read_count{0}, buffer{nullptr} {
  if (std::filesystem::exists(path)) {
    struct stat sb;
    stat(path.c_str(), &sb);
    last_change = (size_t)sb.st_ctime;
    size = std::filesystem::file_size(path);
    nchunks = ceil((double)size / CHUNK_SIZE);
  }
}

FileEntry::FileEntry(const std::string &path, entry_producer producer,
                     size_t nchunks, size_t last_change, entry_status status)
    : path{path}, producer{producer}, nchunks{nchunks},
      last_change{last_change}, status{status}, read_count{0}, buffer{nullptr} {
  if (std::filesystem::exists(path)) {
    struct stat sb;
    stat(path.c_str(), &sb);
    //last_change = (size_t)sb.st_ctime;
    size = std::filesystem::file_size(path);
    nchunks = ceil((double)size / CHUNK_SIZE);
  }
}

void FileEntry::set_producer(entry_producer producer) {
  this->producer = producer;
}
void FileEntry::set_read_count(size_t read_count) const {
  read_count = read_count;
}
void FileEntry::set_nchunks(size_t nchunks) const { nchunks = nchunks; }
std::string FileEntry::get_path() const { return path; }
bool FileEntry::has_chunk() { return read_count < nchunks; }
size_t FileEntry::get_nchunks() const { return nchunks; }
entry_producer FileEntry::get_producer() const { return producer; }
void FileEntry::set_status(entry_status status) { this->status = status; }
entry_status FileEntry::get_status() const { return status; }
size_t FileEntry::get_last_change() const { return last_change; }
std::tuple<size_t, std::string> FileEntry::get_last_move() const {
  return last_move;
}

std::tuple<std::shared_ptr<char[]>, size_t> FileEntry::next_chunk() {
  // DurationLogger log{"READ_CHUNK"};
  std::ifstream in{path, std::ios::binary};
  in.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
  // lazy init of infos
  if (buffer.get() == nullptr) {
    buffer = std::shared_ptr<char[]>{new char[CHUNK_SIZE]};
    memset(buffer.get(), '\0', CHUNK_SIZE);
  }
  size_t to_read = read_count < (nchunks - 1)
                       ? CHUNK_SIZE
                       : (size - ((nchunks - 1) * CHUNK_SIZE));
  in.seekg(read_count * CHUNK_SIZE);
  in.read(buffer.get(), to_read);
  last_move = std::tuple{read_count, Sha256::getSha256(buffer, to_read)};
  read_count++;
  return std::tuple(buffer, to_read);
}

void FileEntry::retrieve_chunk() {
  // DurationLogger log{"READ_CHUNK_FROM_SERVER"};
  std::shared_ptr<RestClient> rest_client = RestClient::getInstance();
  std::filesystem::path new_path{path};
  std::filesystem::create_directories(new_path.parent_path().string());
  std::string tmp_path{path};
  std::ofstream out{path, std::ios::app | std::ios::binary};
  out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  if (buffer.get() == nullptr) {
    buffer = std::shared_ptr<char[]>{new char[CHUNK_SIZE]};
    memset(buffer.get(), '\0', CHUNK_SIZE);
  }
  std::string target{std::to_string(read_count) + "/" +
                     macaron::Base64::Encode(path)};
  std::vector<char> response = rest_client->get_chunk(target);
  out.write(response.data(), response.size());
  read_count++;
}

json FileEntry::to_json() {
  json entry = {{"path", path},
                {"last_change", last_change},
                {"status", status},
                {"producer", producer},
                {"nchunks", nchunks}};
  return entry;
}

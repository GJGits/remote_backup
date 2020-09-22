#include "../../include/services/chunk-service.hpp"

std::shared_ptr<ChunkService> ChunkService::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<ChunkService>(new ChunkService{});
    instance->chunk_repository = ChunkRepository::getInstance();
  }
  return instance;
}

void ChunkService::file_chunk_add(const PostChunkDTO &post_chunk) {
  if (Sha256::getSha256(*post_chunk.getchunk_body())
          .compare(post_chunk.getchunk_hash()) == 0) {
    ChunkEntity chunk_ent{post_chunk};
    std::filesystem::create_directories(post_chunk.getfile_path());
    std::ofstream out_file{
        post_chunk.getfile_path() + "/" + post_chunk.getfile_name() + "__" +
            std::to_string(post_chunk.getchunk_id()) + ".chk",
        std::ios::binary};
    if (out_file.is_open()) {
      out_file.write(
          reinterpret_cast<char *>(post_chunk.getchunk_body()->data()),
          post_chunk.getchunk_size());
      chunk_repository->add_or_update_Chunk(chunk_ent);
      return;
    }

    throw FileNotOpened();
  }
  throw ChunkCorrupted();
}

void ChunkService::file_chunk_update(const PutChunkDTO &put_chunk) {
  if (Sha256::getSha256(*put_chunk.getchunk_body())
          .compare(put_chunk.getchunk_hash()) == 0) {
    ChunkEntity chunk_ent{put_chunk};
    std::string chk_fname{put_chunk.getfile_path() + "/" +
                          put_chunk.getfile_name() + "__" +
                          std::to_string(put_chunk.getchunk_id()) + ".chk"};
    // todo: verificare che questa scrittura butta tutto e riscrive
    std::ofstream out_file{chk_fname, std::ios::binary};
    if (!out_file.is_open()) {
      out_file.write(
          reinterpret_cast<char *>(put_chunk.getchunk_body()->data()),
          put_chunk.getchunk_size());
      chunk_repository->add_or_update_Chunk(chunk_ent);
    }

    throw FileNotOpened();
  }
  throw ChunkCorrupted();
}

std::shared_ptr<std::vector<char>>
ChunkService::file_chunk_get(const GetChunkDTO &get_chunk) {
  std::string fname{"../../filesystem/" + get_chunk.get_subject().get_sub() +
                    "/" + get_chunk.getfile_name() + "/" +
                    get_chunk.getfile_name() + "__" +
                    std::to_string(get_chunk.getchunk_id()) + ".chk"};
  std::ifstream ifile{fname};
  std::shared_ptr<std::vector<char>> buff{};
  ChunkEntity chunk_ent{get_chunk};
  ifile.read(buff->data(), get_chunk.getchunk_size());
  return buff;
}

void ChunkService::file_chunk_delete_service(const DeleteChunkDTO &del_chunk) {

  ChunkEntity chunk_ent{del_chunk};
  chunk_repository->delete_chunks(chunk_ent);
}

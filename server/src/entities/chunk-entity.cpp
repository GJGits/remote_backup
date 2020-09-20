#include "../../include/entities/chunk-entity.hpp"

ChunkEntity::ChunkEntity(const PostChunkDTO &post_chunk)
    : username{std::move(post_chunk.get_subject().get_sub())},
      db{post_chunk.get_subject().get_db_id()},
      id_chunk{post_chunk.getchunk_id()}, hash_chunk{std::move(
                                              post_chunk.getchunk_hash())},
      path_file{std::move(post_chunk.getfile_path())},
      size_chunk{post_chunk.getchunk_size()},
      last_mod{post_chunk.gettimestamp_locale()} {
  std::move(post_chunk.getchunk_body().get()->begin(),
            post_chunk.getchunk_body().get()->end(),
            std::back_inserter(*chunk_body));
};

ChunkEntity::ChunkEntity(const PutChunkDTO &put_chunk)
    : username(std::move(put_chunk.get_subject().get_sub())),
      db{put_chunk.get_subject().get_db_id()},
      id_chunk{put_chunk.getchunk_id()}, hash_chunk{std::move(
                                             put_chunk.getchunk_hash())},
      path_file{std::move(put_chunk.getfile_path())},
      size_chunk{put_chunk.getchunk_size()},
      last_mod{put_chunk.gettimestamp_locale()} {
  std::move(put_chunk.getchunk_body().get()->begin(),
            put_chunk.getchunk_body().get()->end(),
            std::back_inserter(*chunk_body));
};

ChunkEntity::ChunkEntity(const DeleteChunkDTO &del_chunk)
    : username{std::move(del_chunk.get_subject().get_sub())},
      id_chunk{del_chunk.getchunk_id()}, path_file{std::move(
                                             del_chunk.getfile_path())},
      last_mod{del_chunk.gettimestamp_locale()} {};

ChunkEntity::ChunkEntity(const GetChunkDTO &get_chunk)
    : username{std::move(get_chunk.getusername())},
      id_chunk{get_chunk.getchunk_id()}, path_file{std::move(
                                             get_chunk.getfile_path())} {};

std::string ChunkEntity::getUsername() const { return username; };
size_t ChunkEntity::getDb() const { return db; }
int ChunkEntity::getIdChunk() const { return id_chunk; };
std::string ChunkEntity::getHashChunk() const { return hash_chunk; };
std::string ChunkEntity::getPathFile() const { return path_file; };
int ChunkEntity::getSizeChunk() const { return size_chunk; };
int ChunkEntity::getLastMod() const { return last_mod; };
int ChunkEntity::getSizeFile() const { return size_file; };
void ChunkEntity::setSizeFile(int val) { size_file = val; };
std::shared_ptr<std::vector<char>> ChunkEntity::getchunk_body() const {
  return chunk_body;
}
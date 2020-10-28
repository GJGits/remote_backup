#include "../../include/entities/chunk-entity.hpp"

ChunkEntity::ChunkEntity(const PostChunkDTO &post_chunk)
    : subject{post_chunk.get_subject()}, id_chunk{post_chunk.getchunk_id()},
      hash_chunk{std::move(post_chunk.getchunk_hash())},
      file_name{std::move(post_chunk.getfile_name())},
      last_mod{post_chunk.gettimestamp_locale()},
      num_chunks{post_chunk.getnum_chunks()}{
  std::move(post_chunk.getchunk_body().get()->begin(),
            post_chunk.getchunk_body().get()->end(),
            std::back_inserter(*chunk_body));
};


ChunkEntity::ChunkEntity(const GetChunkDTO &get_chunk)
    : subject{get_chunk.get_subject()}, id_chunk{get_chunk.getchunk_id()},
      path_file{std::move(get_chunk.getfile_path())} {};



Subject ChunkEntity::get_subject() const { return subject; };
int ChunkEntity::getIdChunk() const { return id_chunk; };
std::string ChunkEntity::getHashChunk() const { return hash_chunk; };
std::string ChunkEntity::getPathFile() const { return path_file; };
std::string ChunkEntity::getFileName() const { return file_name; };
int ChunkEntity::getLastMod() const { return last_mod; };
int ChunkEntity::getNumChunks() const { return num_chunks; };
int ChunkEntity::getSizeFile() const { return size_file; };
void ChunkEntity::setSizeFile(int val) { size_file = val; };
std::shared_ptr<std::vector<char>> ChunkEntity::getchunk_body() const {
  return chunk_body;
}
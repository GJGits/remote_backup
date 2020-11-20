#include "../../include/entities/chunk-entity.hpp"

ChunkEntity::ChunkEntity(const PostChunkDTO &post_chunk)
    : subject{post_chunk.get_subject()}, id_chunk{post_chunk.getchunk_id()},
      file_name{std::move(post_chunk.getfile_name())},
      num_chunks{post_chunk.getnum_chunks()}{};

Subject ChunkEntity::get_subject() const { return subject; };
int ChunkEntity::getIdChunk() const { return id_chunk; };
std::string ChunkEntity::getPathFile() const { return path_file; };
std::string ChunkEntity::getFileName() const { return file_name; };
int ChunkEntity::getNumChunks() const { return num_chunks; };

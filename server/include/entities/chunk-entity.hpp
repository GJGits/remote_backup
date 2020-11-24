#pragma once
#include <string>
#include "../dtos/post_chunk_dto.hpp"


class ChunkEntity {
private:
    Subject subject;
    int id_chunk;
    std::string path_file;
    std::string file_name;
    int num_chunks;
    int chunk_size;

public:
    ChunkEntity(const PostChunkDTO &post_chunk);

    Subject get_subject() const;
    int getIdChunk() const;
    std::string getPathFile() const;
    std::string getFileName() const;
    int getNumChunks() const;
    int getChunkSize() const;
};

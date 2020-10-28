#pragma once
#include <string>
#include "../common/logger.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"

class ChunkEntity {
private:
    Subject subject;
    int id_chunk;
    std::string path_file;
    std::string file_name;
    int last_mod;
    int num_chunks;
    std::shared_ptr<std::vector<char>> chunk_body{new std::vector<char>{}};

public:
    ChunkEntity(const PostChunkDTO &post_chunk);

    ChunkEntity(const GetChunkDTO &get_chunk);

    Subject get_subject() const;
    int getIdChunk() const;
    std::string getPathFile() const;
    std::string getFileName() const;
    int getLastMod() const;
    int getNumChunks() const;
    std::shared_ptr<std::vector<char>> getchunk_body() const;

};
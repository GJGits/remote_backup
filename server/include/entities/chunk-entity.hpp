#pragma once
#include <string>
#include "../common/logger.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/delete_chunk_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"

class ChunkEntity {
private:
    Subject subject;
    int id_chunk;
    std::string hash_chunk;
    std::string path_file;
    int size_chunk;
    int last_mod;
    int num_chunks;
    int size_file;
    std::shared_ptr<std::vector<char>> chunk_body{new std::vector<char>{}};

public:
    ChunkEntity(const PostChunkDTO &post_chunk);


    ChunkEntity(const DeleteChunkDTO &del_chunk);

    ChunkEntity(const GetChunkDTO &get_chunk);

    Subject get_subject() const;
    int getIdChunk() const;
    std::string getHashChunk() const;
    std::string getPathFile() const;
    int getSizeChunk() const;
    int getLastMod() const;
    int getSizeFile() const;
    int getNumChunks() const;

    void setSizeFile(int val);
    std::shared_ptr<std::vector<char>> getchunk_body() const;

};
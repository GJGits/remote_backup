#pragma once
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../common/sha256.hpp"
#include "../repositories/chunk-repository.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "../exceptions/exceptions.hpp"
#include <memory>
#include "../common/duration.hpp"
#include "../repositories/chunk-repository.hpp"


class ChunkService: public Singleton<ChunkService> {
private:
    friend class Singleton;
    ChunkService(){}

public:
    std::mutex mtx;
    void file_chunk_add(const PostChunkDTO &post_chunk);
    size_t file_chunk_get(const GetChunkDTO &get_chunk);

    ~ChunkService() {}
};

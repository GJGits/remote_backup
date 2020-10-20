#pragma once
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/put_chunk_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/delete_chunk_dto.hpp"
#include "../common/sha256.hpp"
#include "../repositories/chunk-repository.hpp"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "../exceptions/exceptions.hpp"
#include "../common/jwt.hpp"
#include "../common/utility.hpp"
#include <ctime>
#include <memory>
#include "../common/constants.hpp"

#include "../repositories/chunk-repository.hpp"


class ChunkService {
private:
    static inline std::shared_ptr<ChunkService> instance{nullptr};
    std::shared_ptr<ChunkRepository> chunk_repository;
public:
    std::mutex mtx;
    static std::shared_ptr<ChunkService> getInstance();
    void file_chunk_add(const PostChunkDTO &post_chunk);
    size_t file_chunk_get(const GetChunkDTO &get_chunk);
    void file_chunk_put(const PutChunkDTO &put_chunk);

    ~ChunkService() {}
};

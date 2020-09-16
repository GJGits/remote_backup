#pragma once
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/chunk-entity.hpp"
#include "../exceptions/exceptions.hpp"

class ChunkRepository {
private:
public:

    bool add_or_update_Chunk(const ChunkEntity &chunk);
    std::string get_Chunk(const ChunkEntity &chunk);
    bool delete_chunks(const ChunkEntity &chunk);
};
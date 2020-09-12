#pragma once
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/chunk-entity.hpp"
#include "../exceptions/exceptions.hpp"

class ChunkRepository {
private:
public:


    bool getFilePath(const ChunkEntity &chunk);
    bool addChunk(const ChunkEntity &chunk);
    bool add_or_update_FileInfo(const ChunkEntity &chunk);
    int getFileSize(const ChunkEntity &chunk);
    bool updateChunk(const ChunkEntity &chunk);
    bool delete_chunks(const ChunkEntity &chunk);
};
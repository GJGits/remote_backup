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
    bool addFileInfo(const ChunkEntity &chunk);
    int getFileSize(const ChunkEntity &chunk);
    bool updateFileInfo(const ChunkEntity &chunk);
    bool updateChunk(const ChunkEntity &chunk);
    bool delete_chunks(const ChunkEntity &chunk);
};
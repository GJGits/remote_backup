#pragma once
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/chunk-entity.hpp"
#include "../exceptions/exceptions.hpp"

class ChunkRepository {
private:
public:
    //bool insertUser(const UserEntity &user);
    //UserEntity getUserByUsername(const std::string &username);
    //bool deleteUserByUsername(const std::string &username);
    bool getFilePath(const ChunkEntity &chunk);
    bool addChunk(const ChunkEntity &chunk);
    bool addFileInfo(const ChunkEntity &chunk);
};
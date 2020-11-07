#pragma once
#include "../common/utility.hpp"
#include "../entities/chunk-entity.hpp"
#include "../entities/db-connect.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/singleton.hpp"
#include "repository.hpp"


class ChunkRepository: public Singleton<ChunkRepository> , public Repository{
private:
    friend class Singleton;

public:
  bool add_or_update_Chunk(const ChunkEntity &chunk);
};
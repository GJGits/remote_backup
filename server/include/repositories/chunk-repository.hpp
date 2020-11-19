#pragma once
#include "../entities/chunk-entity.hpp"
#include "../common/singleton.hpp"
#include "repository.hpp"


class ChunkRepository: public Singleton<ChunkRepository> , public Repository{
private:
    friend class Singleton;

public:
  bool add_or_update_Chunk(const ChunkEntity &chunk);
};
#pragma once
#include "../common/utility.hpp"
#include "../entities/chunk-entity.hpp"
#include "../entities/db-connect.hpp"
#include "../exceptions/exceptions.hpp"
#include "common-repository-function.hpp"

class ChunkRepository {
private:
  static inline std::shared_ptr<ChunkRepository> instance{nullptr};

public:
  bool add_or_update_Chunk(const ChunkEntity &chunk);
  static std::shared_ptr<ChunkRepository> getInstance();
};
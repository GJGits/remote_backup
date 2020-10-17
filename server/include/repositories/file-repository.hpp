#pragma once
#include <memory>
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/file-entity.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/utility.hpp"

class FileRepository {
private:
  static inline std::shared_ptr<FileRepository> instance{nullptr};

public:
  bool deleteFile(const FileEntity &chunk);
  static std::shared_ptr<FileRepository> getInstance();
};
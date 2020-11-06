#pragma once
#include <memory>
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/file-entity.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/utility.hpp"
#include "repository.hpp"
#include "../common/singleton.hpp"

class FileRepository: public Singleton<FileRepository> , public Repository{
private:
    friend class Singleton;

public:
  bool deleteFile(const FileEntity &chunk);
};
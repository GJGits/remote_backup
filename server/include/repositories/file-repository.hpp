#pragma once
#include <memory>
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/file-entity.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/utility.hpp"
#include "common-repository-function.hpp"
#include "../common/singleton.hpp"

class FileRepository: public Singleton<FileRepository> {
private:
    friend class Singleton;

public:
  bool deleteFile(const FileEntity &chunk);
};
#pragma once
#include <optional>

#include "../entities/db-connect.hpp"
#include "../entities/file-entity.hpp"
#include "../exceptions/exceptions.hpp"

class FileRepository {
private:
public:
    bool deleteFile(const FileEntity &chunk);



};
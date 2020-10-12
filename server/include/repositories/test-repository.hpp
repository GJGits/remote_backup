#pragma once
#include <memory>
#include <optional>
#include "../dtos/get_test_database_dto.hpp"
#include "../dtos/get_test_filesystem_filename_dto.hpp"

#include "../entities/db-connect.hpp"
#include "../exceptions/exceptions.hpp"

using json = nlohmann::json;



class TestRepository {
private:
    static inline std::shared_ptr<TestRepository> instance{nullptr};

public:
    void reset_db();
    static std::shared_ptr<TestRepository> getInstance();
    json getTestDatabaseTableChunks(const GetTestDatabaseDTO &get_test_database);
    json getTestDatabaseTableUsers(const GetTestDatabaseDTO &get_test_database);
    json getTestDatabaseTabledbstatus(const GetTestDatabaseDTO &get_test_database);
    json getTestDatabaseTableUsersDB(const GetTestDatabaseDTO &get_test_database);
    json getTestDatabaseFilesystemFilename(const GetTestFilesystemFilenameDTO &get_test_filesystem_filename);

};
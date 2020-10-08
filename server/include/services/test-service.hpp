#pragma once
#include "../repositories/test-repository.hpp"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "../exceptions/exceptions.hpp"
#include "../common/jwt.hpp"
#include <ctime>
#include "../common/constants.hpp"


class TestService {
private:
    static inline std::shared_ptr<TestService> instance{nullptr};
    std::shared_ptr<TestRepository> test_repository;
public:


    static std::shared_ptr<TestService> getInstance();


    const json getTestDatabaseTable(const GetTestDatabaseDTO &get_test_database);
    const json getTestFilesystemFilename(const GetTestFilesystemFilenameDTO &get_test_filesystem_filename);
    ~TestService() {}

};
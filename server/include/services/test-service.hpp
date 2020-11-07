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
#include "../common/json.hpp"

class TestService: public Singleton<TestService> {
private:
    friend class Singleton;
    TestService(){}
public:


    void cleanDB();
    const json getTestDatabaseTable(const GetTestDatabaseDTO &get_test_database);
    const json getTestFilesystemFilename(const GetTestFilesystemFilenameDTO &get_test_filesystem_filename);
    ~TestService() {}

};
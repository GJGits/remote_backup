#include "../../include/services/test-service.hpp"

std::shared_ptr<TestService> TestService::getInstance() {
    if (instance.get() == nullptr) {
        instance = std::shared_ptr<TestService>(new TestService{});
        instance->test_repository = TestRepository::getInstance();
    }
    return instance;
}

const json TestService::getTestDatabaseTable(const GetTestDatabaseDTO &get_test_database) {
    if(get_test_database.get_table_name().compare("chunks")==0)
        return test_repository->getTestDatabaseTableChunks(get_test_database);
    else if (get_test_database.get_table_name().compare("users")==0)
        return test_repository->getTestDatabaseTableUsers(get_test_database);
    else if (get_test_database.get_table_name().compare("db_status")==0)
        return test_repository->getTestDatabaseTabledbstatus(get_test_database);
    else if (get_test_database.get_table_name().compare("UsersDB")==0)
        return test_repository->getTestDatabaseTableUsersDB(get_test_database);


    throw UknownError();
}


const json TestService::getTestFilesystemFilename(const GetTestFilesystemFilenameDTO &get_test_filesystem_filename) {

    return test_repository->getTestDatabaseFilesystemFilename(get_test_filesystem_filename);

}
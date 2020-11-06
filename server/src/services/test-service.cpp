#include "../../include/services/test-service.hpp"


const json TestService::getTestDatabaseTable(const GetTestDatabaseDTO &get_test_database) {
    std::shared_ptr<TestRepository> test_repository = TestRepository::getInstance();
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

    std::string prefix{"../../filesystem/"+get_test_filesystem_filename.get_username()+"/"};
    std::string path;
    json j;
    j["entries"] = json::array();
    json j_single_path = {};

    int i = 0;
    while(1){
        path = prefix + get_test_filesystem_filename.get_file_name() + "/" + get_test_filesystem_filename.get_file_name() + "__" + std::to_string(i) + ".chk";

        if(std::filesystem::exists(path)) {
            i++;
        }
        else{
            break;
        }
    }

    j["last_page"] = (int) std::ceil(i/ENTRIES_PAGE);

    for(i = get_test_filesystem_filename.getpage_num()*ENTRIES_PAGE ; i < (get_test_filesystem_filename.getpage_num()+1)*ENTRIES_PAGE ; i++){
        path = prefix + get_test_filesystem_filename.get_file_name() + "/" + get_test_filesystem_filename.get_file_name() + "__" + std::to_string(i) + ".chk";
        if(std::filesystem::exists(path)) {
            j_single_path["c_id"] = i;
            j_single_path["c_size"] = std::filesystem::file_size(path);
            j["entries"].push_back(j_single_path);
        } else
            break;
    }

    return j;

}

void TestService::cleanDB() {
    std::shared_ptr<TestRepository> test_repository = TestRepository::getInstance();
    test_repository->reset_db();
}
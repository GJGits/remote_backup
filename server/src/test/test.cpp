#include "../../include/test/test.hpp"

void printResult(int tests, int errors)
{
    if (errors != 0)
        std::clog << "passed [" << (tests - errors) << "/" << tests << "] KO!\n";
    else
        std::clog << "all tests OK!\n";
}

void testSQLCRUD()
{

    std::clog << "##### TEST SQL CRUD #####\n";
    int tests = 2;
    int errors = 0;
    
    // test insert
    std::string username("Pippo");
    std::string hashedPassword("hashed_password");
    unsigned int salt = 123456;
    UserEntity entity{username, hashedPassword, salt};
    UserRepository userRepo{};
    bool insRes = userRepo.insertUser(entity);
    
    if(!insRes) {
        errors++;
        std::clog << "expected true got false\n";
    }
    // test select
    std::optional<UserEntity> retrieved = userRepo.getUserByUsername(std::string{"Pippo"});

    if(!retrieved.has_value()) {
        errors++;
        std::clog << "select: expected true got false\n";
    }

    // test delete
    userRepo.deleteUserByUsername(username);

    printResult(tests, errors);
}



void testAll()
{
    std::clog << "Running tests...\n";
    testSQLCRUD();
}

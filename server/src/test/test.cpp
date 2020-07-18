#include "../lib/test.hpp"

void printResult(int tests, int errors)
{
    if (errors != 0)
        std::clog << "passed [" << (tests - errors) << "/" << tests << "] KO!\n";
    else
        std::clog << "all tests OK!\n";
}

void testSQLCRUD()
{
    // test select
    std::clog << "##### TEST SQL CRUD #####\n";
    int tests = 2;
    int errors = 2;
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("SELECT username FROM users WHERE username = ?"))};
    stmt->setString(1, "Pippo");
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    res->next();
    std::string username = std::string { res->getString("username").c_str() };
    std::clog << "username: " << username << "\n";
    if (username.compare("Pippo") != 0) {
        errors++;
        std::clog << "got: " << username << ", expected: Pippo\n";
    }
    // test insert
    con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("INSERT INTO users(id, username, password, salt) VALUES(DEFAULT, ?, ?, ?)"))};
    stmt->setString(1, "Pluto");
    stmt->setString(2, "0001");
    stmt->setInt(3, 5);
    stmt->execute();
    printResult(tests, errors);
}

void testAll()
{
    std::clog << "Running tests...\n";
    testSQLCRUD();
}
#include "../lib/test.hpp"

void printResult(int tests, int errors)
{
    if (errors != 0)
        std::clog << "passed [" << (tests - errors) << "/" << tests << "] KO!\n";
    else
        std::clog << "all tests OK!\n";
}

unsigned int retrieve_id() {
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con;
    con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("SELECT id from users;"))};
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    unsigned int actual = 0;
    unsigned int past = 0;
    if(res->next()){
    	while(1){
	    	actual = res->getUInt(1);
	    	if((actual - past) > 1) // c'è un foto
	    		return past + 1; //inserisco l'elemento nel foro	
	    	past = actual;
	    	if(!res->next())  //se sono tutti pieni e finisce, ritorna il max trovato + 1
	    		return actual + 1;
	    }
    }
    else
    	return 0;
    	
}

void testSQLCRUD()
{

    std::clog << "##### TEST SQL CRUD #####\n";
    int tests = 2;
    int errors = 1;
    unsigned int maxid;
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con;
    con = DBConnect::getConnection();

    maxid = retrieve_id();

    // testing INSERT
    con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("INSERT INTO users(id, username, password, salt) VALUES(?, ?, ?, ?)"))};
    stmt->setUInt(1, maxid);
    stmt->setString(2, "Test_username");
    stmt->setString(3, "0001");
    stmt->setInt(4, 5);
    stmt->execute();
    


    // testing SELECT
    con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("SELECT username FROM users WHERE username = ?"))};
    stmt->setString(1, "Test_username");
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    res->next();
    std::string username = std::string { res->getString("username").c_str() };
    std::clog << "username: " << username << "\n";
    if (username.compare("Test_username") != 0) 
        std::clog << "got: " << username << ", expected: Test_username\n";
    else
    	errors--;
    
    
    // end test function
    printResult(tests, errors);
    
    // clean tests queries
    con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move( con->prepareStatement("DELETE FROM users WHERE username = ?"))};
    stmt->setString(1, "Test_username");
    stmt->execute();
    
}



void testAll()
{
    std::clog << "Running tests...\n";
    testSQLCRUD();
}

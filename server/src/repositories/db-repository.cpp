#include "../../include/repositories/db-repository.hpp"

std::shared_ptr<DBRepository> DBRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<DBRepository>(new DBRepository{});
  }
  return instance;
}

int DBRepository::getDBbyUsername(const std::string &username) {

  if (users_db_map.count(username))
    return users_db_map[username];

  if (users_db_map.size() >= 100)
    users_db_map.clear();

  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(0);
    std::string username_escaped = mysqlConn->escapeString(username);

    std::string query = "SELECT db FROM UsersDB WHERE username = '"+username_escaped+"';";

    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};

    res->next();
    int db_selected = res->getInt("db");
    users_db_map[username] = db_selected;
    return db_selected;


}


size_t DBRepository::insertUsernameInDB(const std::string &username) {

  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(0);
    std::string query = "SELECT db FROM db_status GROUP BY db ORDER BY usc, db LIMIT 1;";

    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};
    size_t db_selected = 0;

    if (res->next())
      db_selected = (size_t)res->getInt("db");
    else
        throw MissingElementInDB();

    std::string username_escaped = mysqlConn->escapeString(username);
    std::string db_selected_escaped = mysqlConn->escapeString(std::to_string(db_selected));

    query = "INSERT INTO UsersDB(username,db) VALUES('"+username_escaped+"',"+db_selected_escaped+");";

    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    stmt->execute(query);


    query = "UPDATE db_status SET usc= usc + 1 WHERE db="+db_selected_escaped+"";
    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    stmt->execute(query);

    return db_selected;

}
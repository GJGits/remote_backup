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

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(0);
  if (con->isValid() && !con->isClosed()) {
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move(
        con->prepareStatement("SELECT db FROM UsersDB WHERE username = ?;"))};
    stmt->setString(1, sql::SQLString{username.c_str()});
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    if (res->next()) {
      int db_selected = res->getInt("db");
      users_db_map[username] = db_selected;
      return db_selected;
    }
  }
  throw DatabaseInvalidConnection();
}


size_t DBRepository::insertUsernameInDB(const std::string &username) {

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(0);
  if (con->isValid() && !con->isClosed()) {
    stmt =
        std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
            "SELECT db FROM db_status GROUP BY db ORDER BY usc, db LIMIT 1;"))};

    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    size_t db_selected = 0;

    if (res->next())
      db_selected = (size_t)res->getInt("db");

    stmt =
        std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
            "INSERT INTO UsersDB(username,db) VALUES(?,?);"))};
    stmt->setString(1, sql::SQLString{username.c_str()});
    stmt->setInt(2, db_selected);
    stmt->executeUpdate();

    stmt = std::unique_ptr<sql::PreparedStatement>{std::move(
        con->prepareStatement("UPDATE db_status SET usc= usc + 1 WHERE db=?"))};
    stmt->setInt(1, db_selected);
    stmt->executeUpdate();
    return db_selected;
  }
  throw DatabaseInvalidConnection();
}
#include "../../include/repositories/db-repository.hpp"

int DBRepository::getDBbyUsername(const std::string &username) {

  std::unique_ptr<sql::ResultSet> res;
  std::string query = "SELECT db FROM UsersDB WHERE username = '?';";
  std::list<std::string> entries_of_query;
  entries_of_query.push_back(username);
  res = select_query(query, entries_of_query, 0);
  res->next();
  int db_selected = res->getInt("db");
  return db_selected;
}

size_t DBRepository::insertUsernameInDB(const std::string &username) {

  std::unique_ptr<sql::ResultSet> res;
  std::list<std::string> entries_of_query;
  std::string query = "SELECT db FROM db_status GROUP BY db ORDER BY usc, db LIMIT 1;";
  res = select_query(query, entries_of_query, 0);

  if (!res->next())
      throw MissingElementInDB();

  size_t db_selected = (size_t)res->getInt("db");

  query = "INSERT INTO UsersDB(username,db) VALUES('?'," + std::to_string(db_selected) + ");";
  entries_of_query.push_back(username);
  update_query(query,entries_of_query,0);

  query ="UPDATE db_status SET usc= usc + 1 WHERE db=" + std::to_string(db_selected) + "";
  update_query(query,entries_of_query,0);

  return db_selected;
}
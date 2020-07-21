#include "../lib/repositories/user-repository.hpp"

bool UserRepository::insertUser(const UserEntity &user) {

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  try {

    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();

    stmt = std::unique_ptr<
        sql::PreparedStatement>{std::move(con->prepareStatement(
        "INSERT INTO users(username, hashed_password, salt) VALUES(?, ?, ?)"))};

    stmt->setString(1, sql::SQLString{user.getUsername().c_str()});
    stmt->setString(2, sql::SQLString{user.getHashedPassword().c_str()} );
    stmt->setUInt(3, user.getSalt());
    return stmt->executeUpdate() == 1 ? true : false;

  } catch (sql::SQLException &e) {
    std::clog << "insert mysql error: " << e.getErrorCode();
  }

  return false;
}

std::optional<UserEntity>
UserRepository::getUserByUsername(const std::string &username) {

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  try {

    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{
        std::move(con->prepareStatement("SELECT username, hashed_password, "
                                        "salt FROM users WHERE username = ?"))};
    stmt->setString(1, username);
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    if (res->next()) {
      std::string hashed_password =
          std::move(res->getString("hashed_password"));
      unsigned int salt = res->getInt("salt");
      UserEntity entity{username, hashed_password, salt};
      return std::optional<UserEntity>{entity};
    }

    return std::nullopt;

  } catch (sql::SQLException &e) {
    std::clog << "select mysql error: " << e.getErrorCode();
  }

  return std::nullopt;
}

bool UserRepository::deleteUserByUsername(const std::string &username) {

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  try {

    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move(
        con->prepareStatement("DELETE from users WHERE username = ?"))};
    stmt->setString(1, username);
    return stmt->executeUpdate() == 1 ? true : false;

  } catch (sql::SQLException &e) {
    std::clog << "delete error: " << e.getErrorCode();
  }

  return false;
}
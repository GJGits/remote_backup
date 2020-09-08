#include "../../include/repositories/user-repository.hpp"

bool UserRepository::insertUser(const UserEntity &user) {

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  try {

    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();

    stmt = std::unique_ptr<
        sql::PreparedStatement>{std::move(con->prepareStatement(
        "INSERT INTO users(username, hashed_password, salt) VALUES(?, ?, ?)"))};

    stmt->setString(1, sql::SQLString{user.getUsername().c_str()});
    stmt->setString(2, sql::SQLString{user.getHashedPassword().c_str()});
    stmt->setUInt(3, user.getSalt());

      return stmt->executeUpdate() == 1 ? true : false;

  } catch (sql::SQLException &e) {
    //std::clog << "insert mysql error\n";
    Logger::log(std::string{"insert mysql error("} +
                std::to_string(e.getErrorCode()) + std::string{")"});
    // ce la caviamo con una generica not
    return false;
  }

  return false;
}

UserEntity
UserRepository::getUserByUsername(const std::string &username) {

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  try {

    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{
        std::move(con->prepareStatement("SELECT username, hashed_password, "
                                        "salt, hashed_status FROM users WHERE username = ?"))};
    stmt->setString(1, username);
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    if (res->next()) {
      std::string hashed_password =
          std::move(res->getString("hashed_password"));
      unsigned int salt = res->getInt("salt");
      UserEntity entity{username, hashed_password, salt};
      return entity;
    }
      throw UsernameNotExists();

  } catch (sql::SQLException &e) {
    //std::clog << "select mysql error\n";
    Logger::log(std::string{"select mysql error("} +
                std::to_string(e.getErrorCode()) + std::string{")"});
    // ce la caviamo con un generico not found
      throw UknownError();

  }

    throw UknownError();
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
    //std::clog << "delete mysql error\n";
    Logger::log(std::string{"delete mysql error("} +
                std::to_string(e.getErrorCode()) + std::string{")"});
    return false;
  }

  return false;
}


//select * from chunks where username = "aldo" and path="a.txt" order by path,id;

std::string UserRepository::update_hashed_status(const std::string &username){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;

    try {

        std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
        std::clog << "e0\n";
        stmt = std::unique_ptr<
               sql::PreparedStatement>{std::move(con->prepareStatement(
                "SELECT hash FROM chunks WHERE username = ? ORDER BY path,id;"))};

        stmt->setString(1, sql::SQLString{username.c_str()});
        std::clog << "e1\n";

        res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
        std::clog << "e2\n";

        std::string hashed_status_concat;
        while (res->next()) {
            hashed_status_concat.append(std::move(res->getString("hash")));
        }
        std::clog << "e3\n";

        std::clog << hashed_status_concat << "\n";
        std::vector<char> data(hashed_status_concat.begin(), hashed_status_concat.end());
        std::clog << "e4\n";

        return Sha256::getSha256(data);

    } catch (sql::SQLException &e) {
        //std::clog << "insert mysql error\n";
        Logger::log(std::string{"insert mysql error("} +
                    std::to_string(e.getErrorCode()) + std::string{")"});
        // ce la caviamo con una generica not
        return "Error";
    }
    //todo:: mettere per bene i try catch
    return "error";
}
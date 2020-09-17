#include "../../include/repositories/db-repository.hpp"



std::shared_ptr<DBRepository> DBRepository::getInstance() {
    if (instance.get() == nullptr) {
        instance = std::shared_ptr<DBRepository>(new DBRepository{});
    }
    return instance;
}



int DBRepository::getDBbyUsername(std::string username) {

    if (users_db_map.count(username))
        return users_db_map[username];

    if (users_db_map.size()>=100)
        users_db_map.clear();

    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection(0);
    if (con->isValid() && !con->isClosed()) {
        stmt = std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
                "SELECT db FROM UsersDB WHERE username = ?;"))};
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



int DBRepository::insertUsernameInDB(std::string username) {

    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection(0);
    if (con->isValid() && !con->isClosed()) {
        stmt = std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement( "SELECT db,COUNT(db) mycount FROM UsersDB GROUP BY db ORDER BY mycount;"))};

        res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
        int db_selected;

        if (res->next())
            db_selected = res->getInt("db");

        std::clog << db_selected << "\n";
        stmt = std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
                "INSERT INTO UsersDB(username,db) VALUES(?,?);"))};
        stmt->setString(1, sql::SQLString{username.c_str()});
        stmt->setInt(2,db_selected);
        stmt->executeUpdate();
        return db_selected;
    }
    throw DatabaseInvalidConnection();
}
#include "../lib/mysql/db-connect.hpp"

DBConnect* DBConnect::instance = nullptr;

std::shared_ptr<sql::Connection>
DBConnect::getConnection()
{
    if (DBConnect::instance == nullptr) {
        DBConnect::instance = new DBConnect();
    }
    int scelta = DBConnect::instance->index;
    DBConnect::instance->index == 15 ? 0 : DBConnect::instance->index++;
    std::clog << "Restituita connection: @" << scelta << "\n";
    return DBConnect::instance->connections[scelta];
}
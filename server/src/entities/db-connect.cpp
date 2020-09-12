#include "../../include/entities/db-connect.hpp"

DBConnect *DBConnect::instance = nullptr;
std::shared_ptr<sql::Connection> DBConnect::getConnection() {
  std::unique_lock lk{DBConnect::m};
  if (DBConnect::instance == nullptr) {
    DBConnect::instance = new DBConnect();
    sql::Driver *driver = get_driver_instance();
    // TODO: convertire 16 con il campo max_connections letto da file
    for (int i = 0; i < 16; i++) {
      try {
        sql::Connection *con;
        con =
            driver->connect("tcp://remote_backup_db_1:3306", "root", "example");
        con->setSchema("db_utenti");
        DBConnect::instance->connections[i] =
            std::shared_ptr<sql::Connection>{con};
      } catch (sql::SQLException &e) {
        // 1. print to debug
        std::clog << " (MySQL error code: " << e.getErrorCode();
        // 2. log to a file
        Logger::log(" (MySQL error code: " + e.getErrorCode());
        // 3. do something
      }
    }
  }

  DBConnect::instance->index =
      DBConnect::instance->index == 16 ? 0 : DBConnect::instance->index;
  int scelta = DBConnect::instance->index;
  DBConnect::instance->index++;

  std::clog << "Restituita connection: @" << scelta << "\n";

  return instance->connections[scelta];
}
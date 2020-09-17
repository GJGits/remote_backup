#include "../../include/entities/db-connect.hpp"

DBConnect *DBConnect::instance = nullptr;
std::shared_ptr<sql::Connection> DBConnect::getConnection(size_t db_selected) {
  std::unique_lock lk{DBConnect::m};
  if (DBConnect::instance == nullptr) {
    DBConnect::instance = new DBConnect();
    sql::Driver *driver = get_driver_instance();
      for (int i = 0; i < 4; i++) {
          sql::Connection *con;
          con = driver->connect("tcp://remote_backup_db_1:3306", "root", "example");
          con->setSchema("db_utenti");
          DBConnect::instance->connections_map[0][i] = std::shared_ptr<sql::Connection>{con};
      }
      for (int i = 0; i < 4; i++) {

        sql::Connection *con;
        con = driver->connect("tcp://remote_backup_db_2:3306", "root", "example");
        con->setSchema("db_utenti");
        DBConnect::instance->connections_map[1][i] = std::shared_ptr<sql::Connection>{con};

      }
      for (int i = 0; i < 4; i++) {

          sql::Connection *con;
          con = driver->connect("tcp://remote_backup_db_3:3306", "root", "example");
          con->setSchema("db_utenti");
          DBConnect::instance->connections_map[2][i] = std::shared_ptr<sql::Connection>{con};

      }
      for (int i = 0; i < 4; i++) {
              sql::Connection *con;
              con = driver->connect("tcp://remote_backup_db_4:3306", "root", "example");
              con->setSchema("db_utenti");
              DBConnect::instance->connections_map[3][i] = std::shared_ptr<sql::Connection>{con};
      }
  }


  DBConnect::instance->indexes[db_selected] =
      DBConnect::instance->indexes[db_selected] == 4 ? 0 : DBConnect::instance->indexes[db_selected];
  int scelta = DBConnect::instance->indexes[db_selected];
  DBConnect::instance->indexes[db_selected]++;


  std::clog << "Restituita connection: @" << scelta << " del db: @" << db_selected << "\n";

  return instance->connections_map[db_selected][scelta];
}

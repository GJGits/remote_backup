#include "../../include/entities/db-connect.hpp"


DBConnect *DBConnect::instance = nullptr;
std::shared_ptr<sql::mysql::MySQL_Connection> DBConnect::getConnection(size_t db_selected) {
  std::unique_lock lk{DBConnect::m};
  if (DBConnect::instance == nullptr) {
    DBConnect::instance = new DBConnect();
    sql::Driver *driver = get_driver_instance();
    for (size_t db_id = 0; db_id < 4; db_id++) {
      for (size_t conn_num = 0; conn_num < 4; conn_num++) {
        std::string db_off = std::to_string(db_id + 1) + ":3306";
        sql::Connection *con = driver->connect(
            "tcp://remote_backup_db_" + db_off, "root", "example");
        con->setSchema("db_utenti");


          std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn(dynamic_cast<sql::mysql::MySQL_Connection*>(con));

        DBConnect::instance->connections_map[db_id][conn_num] = mysqlConn;
      }
    }
  }
  DBConnect::instance->indexes[db_selected] =
      DBConnect::instance->indexes[db_selected] == 4
          ? 0
          : DBConnect::instance->indexes[db_selected];
  int scelta = DBConnect::instance->indexes[db_selected];
  DBConnect::instance->indexes[db_selected]++;

  std::clog << "Restituita connection: @" << scelta << " del db: @"
            << db_selected << "\n";

  return instance->connections_map[db_selected][scelta];
}

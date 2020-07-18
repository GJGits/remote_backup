#include <stdlib.h>
/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"
#include <array>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <mutex>
#include <string>

/**
 * TODO: questa classe deve essere un singleton
 */
class DBConnect {

private:
    sql::Driver* driver;
    std::array<std::shared_ptr<sql::Connection>, 16>
        connections; //  16 = numero massimo connessioni attive su mysql
    int index;
    static DBConnect* instance;
    DBConnect()
        : index { 0 }
    {
        driver = get_driver_instance();
        for (int i = 0; i < 16; i++) {

            sql::Connection* con;
            con = driver->connect("tcp://remote_backup_db_1:3306", "root", "example");
            con->setSchema("db_prova");
            connections[i] = std::shared_ptr<sql::Connection>{con};
        }
    };

public:
    DBConnect(const DBConnect&) = delete; // costruttore copia off
    DBConnect(DBConnect&&) = delete; // costruttore movimento off
    DBConnect& operator=(const DBConnect&) = delete; // assegnazione off
    DBConnect& operator=(DBConnect&&) = delete; // assegnazione movimento off
    static std::shared_ptr<sql::Connection> getConnection();
};

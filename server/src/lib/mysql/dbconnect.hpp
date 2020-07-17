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
#include <string>

/**
 * TODO: questa classe deve essere un singleton
 */
class DBConnect {

private:
  std::string server, username, password, nomeDB;
  sql::Driver *driver;
  std::array<sql::Connection, 16>
      connections; //  16 = numero massimo connessioni attive su mysql
  int index;

public:
  DBConnect(const std::string &server, const std::string &username,
            const std::string &password, const std::string &nomeDB);
  sql::Connection *getConnection();
};
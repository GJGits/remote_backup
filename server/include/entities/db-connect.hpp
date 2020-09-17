#pragma once
#include <cstdio>
#include <iostream>
#include <stdlib.h>
/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "../common/constants.hpp"
#include "../common/logger.hpp"

#include "mysql_connection.h"
#include <array>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include "../repositories/db-repository.hpp"

/**
 * TODO: questa classe deve essere un singleton
 */

class DBConnect {

private:

    std::unordered_map<int, std::array<std::shared_ptr<sql::Connection>, 4>> connections_map;
  std::array<int,3> indexes;
  static DBConnect *instance;
  DBConnect()  {
      indexes = {0, 0, 0};
  }

public:
  inline static std::mutex m;
  DBConnect(const DBConnect &) = delete;            // costruttore copia off
  DBConnect(DBConnect &&) = delete;                 // costruttore movimento off
  DBConnect &operator=(const DBConnect &) = delete; // assegnazione off
  DBConnect &operator=(DBConnect &&) = delete; // assegnazione movimento off

  static std::shared_ptr<sql::Connection> getConnection(size_t db_selected = 0);

};

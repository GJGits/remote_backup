#pragma once
#include <cstdio>
#include <iostream>
#include <stdlib.h>
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
#include "../common/singleton.hpp"

/**
 * TODO: questa classe deve essere un singleton
 */

class DBConnect : public Singleton<DBConnect>{

private:
    friend class Singleton;
    std::unordered_map<int, std::array<std::shared_ptr<sql::mysql::MySQL_Connection>, 4>> connections_map;
  
  std::array<int,4> indexes;
  DBConnect()  {
     // ogni posizione in questo array indica l'utilizzo di una connessione per un determinato db
      indexes = {0, 0, 0, 0};
  }

public:
  inline static std::mutex m;
  DBConnect(const DBConnect &) = delete;            // costruttore copia off
  DBConnect(DBConnect &&) = delete;                 // costruttore movimento off
  DBConnect &operator=(const DBConnect &) = delete; // assegnazione off
  DBConnect &operator=(DBConnect &&) = delete; // assegnazione movimento off

  static std::shared_ptr<sql::mysql::MySQL_Connection> getConnection(size_t db_selected);

};

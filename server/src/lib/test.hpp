#pragma once
#include <iostream>
#include <stdlib.h>
/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"
#include "entities/user-entity.hpp"
#include "repositories/user-repository.hpp"
#include <array>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

void testSQLCRUD();
void testAll();
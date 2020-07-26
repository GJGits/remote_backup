#pragma once
#include <iostream>
#include <stdlib.h>
/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"
#include "../entities/user-entity.hpp"
#include "../repositories/user-repository.hpp"
#include "../common/base64.hpp"
#include "../common/sha256.hpp"
#include "../common/hmac-sha256.hpp"
#include <array>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

void testBase64();
void testSha256();
void testHamacSha256();
void testSQLCRUD();
void testAll();
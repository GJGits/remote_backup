#include "../../include/repositories/test-repository.hpp"

std::shared_ptr<TestRepository> TestRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<TestRepository>{};
  }
  return instance;
}

json TestRepository::getTestDatabaseTableChunks(
    const GetTestDatabaseDTO &get_test_database) {

  json j;
  j["entries"] = json::array();
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = get_test_database.get_subject().get_db_id();
  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn =
      DBConnect::getConnection(db_selected);

  std::string num_page =
      mysqlConn->escapeString(std::to_string(get_test_database.getpage_num()));
  std::string limit_page_num = mysqlConn->escapeString(
      std::to_string((get_test_database.getpage_num() + 1) * ENTRIES_PAGE));

  std::string query =
      "select t1.c_username, t1.c_id, t1.c_hash, t1.c_path, t1.c_size, "
      "t1.c_lastmod, t1.num_chunks, t2.last_page from (select "
      "c_username, c_id, c_hash, c_path, c_size, c_lastmod, num_chunks from "
      "chunks limit " +
      num_page + ", " + limit_page_num +
      ") as t1, (select(ceil((count(distinct c_path) / " +
      std::to_string(ENTRIES_PAGE) +
      "))) -1 as "
      "last_page from chunks) as t2;";

  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};

  json j_single_path = {};

  if (res->rowsCount() > 0) {
    for (int i = 0; i < ENTRIES_PAGE; i++) {
      if (res->next()) {
        j_single_path["c_username"] = res->getString(1);
        j_single_path["c_id"] = res->getInt(2);
        j_single_path["c_hash"] = res->getString(3);
        j_single_path["c_path"] = res->getString(4);
        j_single_path["c_size"] = res->getInt(5);
        j_single_path["c_lastmod"] = res->getInt(6);
        j_single_path["num_chunks"] = res->getInt(7);
        j["entries"].push_back(j_single_path);
        j["last_page"] = res->getInt(8);
      } else
        break;
    }
  } else
    j["last_page"] = 0;

  return j;
}

json TestRepository::getTestDatabaseTableUsers(
    const GetTestDatabaseDTO &get_test_database) {

  json j;
  j["entries"] = json::array();
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = get_test_database.get_subject().get_db_id();
  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn =
      DBConnect::getConnection(db_selected);

  std::string num_page =
      mysqlConn->escapeString(std::to_string(get_test_database.getpage_num()));
  std::string limit_page_num = mysqlConn->escapeString(
      std::to_string((get_test_database.getpage_num() + 1) * ENTRIES_PAGE));

  std::string query =
      "select t1.username, t1.hashed_password, t1.salt, t1.device_1_MAC, "
      "t1.device_2_MAC, t1.device_3_MAC, t1.device_4_MAC, t1.device_5_MAC, "
      "t1.device_6_MAC,t1.device_7_MAC, t1.device_8_MAC,t2.last_page from "
      "(select "
      "username, hashed_password, salt, device_1_MAC, device_2_MAC, "
      "device_3_MAC, device_4_MAC,device_5_MAC, device_6_MAC, device_7_MAC, "
      "device_8_MAC from "
      "users limit " +
      num_page + ", " + limit_page_num +
      ") as t1, (select(ceil((count(distinct username) / " +
      std::to_string(ENTRIES_PAGE) +
      "))) -1 as "
      "last_page from users) as t2;";

  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};

  json j_single_path = {};

  if (res->rowsCount() > 0) {
    for (int i = 0; i < ENTRIES_PAGE; i++) {
      if (res->next()) {
        j_single_path["username"] = res->getString(1);
        j_single_path["hashed_password"] = res->getString(2);
        j_single_path["salt"] = res->getString(3);
        j_single_path["device_1_MAC"] = res->getString(4);
        j_single_path["device_2_MAC"] = res->getString(5);
        j_single_path["device_3_MAC"] = res->getString(6);
        j_single_path["device_4_MAC"] = res->getString(7);
        j_single_path["device_5_MAC"] = res->getString(8);
        j_single_path["device_6_MAC"] = res->getString(9);
        j_single_path["device_7_MAC"] = res->getString(10);
        j_single_path["device_8_MAC"] = res->getString(11);
        j["entries"].push_back(j_single_path);
        j["last_page"] = res->getInt(12);
      } else
        break;
    }
  } else
    j["last_page"] = 0;

  return j;
}

json TestRepository::getTestDatabaseTabledbstatus(
    const GetTestDatabaseDTO &get_test_database) {

  json j;
  j["entries"] = json::array();
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = 0;
  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn =
      DBConnect::getConnection(db_selected);

  std::string num_page =
      mysqlConn->escapeString(std::to_string(get_test_database.getpage_num()));
  std::string limit_page_num = mysqlConn->escapeString(
      std::to_string((get_test_database.getpage_num() + 1) * ENTRIES_PAGE));

  std::string query = "select t1.db, t1.usc, t2.last_page from (select "
                      "db, usc from db_status limit " +
                      num_page + ", " + limit_page_num +
                      ") as t1, (select(ceil((count(distinct db) / " +
                      std::to_string(ENTRIES_PAGE) +
                      "))) -1 as "
                      "last_page from db_status) as t2;";

  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};

  json j_single_path = {};

  if (res->rowsCount() > 0) {
    for (int i = 0; i < ENTRIES_PAGE; i++) {
      if (res->next()) {
        j_single_path["db"] = res->getInt(1);
        j_single_path["usc"] = res->getInt(2);
        j["entries"].push_back(j_single_path);
        j["last_page"] = res->getInt(3);
      } else
        break;
    }
  } else
    j["last_page"] = 0;

  return j;
}

json TestRepository::getTestDatabaseTableUsersDB(
    const GetTestDatabaseDTO &get_test_database) {

  json j;
  j["entries"] = json::array();
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = 0;
  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn =
      DBConnect::getConnection(db_selected);

  std::string num_page =
      mysqlConn->escapeString(std::to_string(get_test_database.getpage_num()));
  std::string limit_page_num = mysqlConn->escapeString(
      std::to_string((get_test_database.getpage_num() + 1) * ENTRIES_PAGE));

  std::string query = "select t1.username, t1.db, t2.last_page from (select "
                      "username, db from UsersDB limit " +
                      num_page + ", " + limit_page_num +
                      ") as t1, (select(ceil((count(distinct username) / " +
                      std::to_string(ENTRIES_PAGE) +
                      "))) -1 as last_page from UsersDB) as t2;";

  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};

  json j_single_path = {};

  if (res->rowsCount() > 0) {
    for (int i = 0; i < ENTRIES_PAGE; i++) {
      if (res->next()) {
        j_single_path["username"] = res->getString(1);
        j_single_path["db"] = res->getInt(2);
        j["entries"].push_back(j_single_path);
        j["last_page"] = res->getInt(3);
      } else
        break;
    }
  } else
    j["last_page"] = 0;

  return j;
}

json TestRepository::getTestDatabaseFilesystemFilename(
    const GetTestFilesystemFilenameDTO &get_test_filesystem_filename) {
  json j;
  j["entries"] = json::array();
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = get_test_filesystem_filename.get_subject().get_db_id();
  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn =
      DBConnect::getConnection(db_selected);

  std::string filename =
      mysqlConn->escapeString(get_test_filesystem_filename.get_file_name());
  std::string num_page = mysqlConn->escapeString(std::to_string(
      get_test_filesystem_filename.getpage_num() * ENTRIES_PAGE));
  std::string limit_page_num = mysqlConn->escapeString(std::to_string(
      (get_test_filesystem_filename.getpage_num() + 1) * ENTRIES_PAGE));

  std::string query = "select t1.c_id, t1.c_size, t2.last_page from (select "
                      "c_id, c_size from chunks where c_path = '" +
                      filename + "' limit " + num_page + ", " + limit_page_num +
                      ") as t1, (select(ceil((count(distinct c_id) / " +
                      std::to_string(ENTRIES_PAGE) +
                      "))) -1 as last_page from chunks) as t2;";
  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};

  json j_single_path = {};

  if (res->rowsCount() > 0) {
    for (int i = 0; i < ENTRIES_PAGE; i++) {
      if (res->next()) {
        j_single_path["c_id"] = res->getInt(1);
        j_single_path["c_size"] = res->getInt(2);
        j["entries"].push_back(j_single_path);
        j["last_page"] = res->getInt(3);
      } else
        break;
    }
  } else
    j["last_page"] = 0;

  return j;
}

void TestRepository::reset_db() {
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn =
      DBConnect::getConnection(0);
  std::string query = "delete from UsersDB;";
  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  stmt->execute(query);
  query = "update db_status set usc = 0;";
  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  stmt->execute(query);

   std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn2 =
      DBConnect::getConnection(1);
  query = "delete from users;";
  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn2->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  stmt->execute(query);
  query = "delete from chunks";
  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn2->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  stmt->execute(query);

  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn3 =
      DBConnect::getConnection(2);
  query = "delete from users;";
  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn3->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  stmt->execute(query);
  query = "delete from chunks";
  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn3->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  stmt->execute(query);

  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn4 =
      DBConnect::getConnection(3);
  query = "delete from users;";
  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn4->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  stmt->execute(query);
  query = "delete from chunks";
  stmt = std::unique_ptr<sql::Statement>{
      std::move(mysqlConn4->createStatement())}; // ricordare al posto di 0, di
                                                // mettere il vero valore
  stmt->execute(query);
}

#include "../../include/repositories/user-repository.hpp"

std::shared_ptr<UserRepository> UserRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<UserRepository>{};
  }
  return instance;
}

size_t UserRepository::insertUser(const UserEntity &user) {

  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->insertUsernameInDB(user.getUsername());
  std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(db_selected);

    std::string username = mysqlConn->escapeString(user.getUsername());
    std::string hashed_password = mysqlConn->escapeString(user.getHashedPassword());
    std::string salt = mysqlConn->escapeString(std::to_string(user.getSalt()));
    std::string device1 = mysqlConn->escapeString(user.get_device_1_MAC());
    std::string device2 = mysqlConn->escapeString(user.get_device_2_MAC());
    std::string device3 = mysqlConn->escapeString(user.get_device_3_MAC());
    std::string device4 = mysqlConn->escapeString(user.get_device_4_MAC());
    std::string device5 = mysqlConn->escapeString(user.get_device_5_MAC());
    std::string device6 = mysqlConn->escapeString(user.get_device_6_MAC());
    std::string device7 = mysqlConn->escapeString(user.get_device_7_MAC());
    std::string device8 = mysqlConn->escapeString(user.get_device_8_MAC());

    std::string query ="INSERT INTO users(username, hashed_password, salt, "
                       "device_1_MAC,device_2_MAC,device_3_MAC,device_4_MAC,device_5_MAC,"
                       "device_6_MAC,device_7_MAC,device_8_MAC) VALUES('"+username+"', '"+hashed_password+"', "+salt+", '"+device1+"', '"+device2+"', '"+device3+"', '"+device4+"', '"+device5+"', '"+device6+"', '"+device7+"', '"+device8+"')";

    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    stmt->execute(query);



  return db_selected;
}

bool UserRepository::UserAlreadyPresent(const std::string &username){
    std::unique_ptr<sql::Statement> stmt;
    std::unique_ptr<sql::ResultSet> res;

    std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(0);
    std::string username_escaped = mysqlConn->escapeString(username);
    std::string query = "Select username from UsersDB where username = '"+username_escaped+"';";
    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};

    if (res->next()) {
        return true;
    }
    return false;
}

void UserRepository::updateUser(const UserEntity &user) {
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(user.getUsername());
    std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(db_selected);


    std::string username = mysqlConn->escapeString(user.getUsername());
    std::string device1 = mysqlConn->escapeString(user.get_device_1_MAC());
    std::string device2 = mysqlConn->escapeString(user.get_device_2_MAC());
    std::string device3 = mysqlConn->escapeString(user.get_device_3_MAC());
    std::string device4 = mysqlConn->escapeString(user.get_device_4_MAC());
    std::string device5 = mysqlConn->escapeString(user.get_device_5_MAC());
    std::string device6 = mysqlConn->escapeString(user.get_device_6_MAC());
    std::string device7 = mysqlConn->escapeString(user.get_device_7_MAC());
    std::string device8 = mysqlConn->escapeString(user.get_device_8_MAC());

  std::string query = "update users set device_1_MAC = '"+device1+"', device_2_MAC = '"+device2+"', device_3_MAC = '"+device3+"', device_4_MAC = '"+device4+"', device_5_MAC = '"+device5+"', device_6_MAC = '"+device6+"',device_7_MAC = '"+device7+"', device_8_MAC = '"+device8+"' where username = '"+username+"'";

    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    stmt->execute(query);
  return;
}

UserEntity UserRepository::getUserByUsername(const std::string &username) {
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(username);
    std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(db_selected);

    std::string username_escaped = mysqlConn->escapeString(username);

    std::string query = "SELECT username,hashed_password, salt, "
      "device_1_MAC,device_2_MAC,device_3_MAC,device_4_MAC,device_5_MAC,device_"
      "6_MAC,device_7_MAC,device_8_MAC FROM users WHERE username = '"+username_escaped+"'";

    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};

  if (res->next()) {
    std::string hashed_password = std::move(res->getString("hashed_password"));
    unsigned int salt = res->getInt("salt");
    UserEntity entity{username, hashed_password, salt};
    std::string mac_1 = std::move(res->getString("device_1_MAC"));
    std::string mac_2 = std::move(res->getString("device_2_MAC"));
    std::string mac_3 = std::move(res->getString("device_3_MAC"));
    std::string mac_4 = std::move(res->getString("device_4_MAC"));
    std::string mac_5 = std::move(res->getString("device_5_MAC"));
    std::string mac_6 = std::move(res->getString("device_6_MAC"));
    std::string mac_7 = std::move(res->getString("device_7_MAC"));
    std::string mac_8 = std::move(res->getString("device_8_MAC"));
    entity.set_device_1_MAC(mac_1);
    entity.set_device_2_MAC(mac_2);
    entity.set_device_3_MAC(mac_3);
    entity.set_device_4_MAC(mac_4);
    entity.set_device_5_MAC(mac_5);
    entity.set_device_6_MAC(mac_6);
    entity.set_device_7_MAC(mac_7);
    entity.set_device_8_MAC(mac_8);

    return entity;
  }
  throw UsernameNotExists();
}

bool UserRepository::deleteUserByUsername(const std::string &username) {

  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(username);
    std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(db_selected);

    std::string username_escaped = mysqlConn->escapeString(username);

    std::string query = "DELETE from users WHERE username = '"+username_escaped+"'";
    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    stmt->execute(query);

  return true;
}



json UserRepository::get_status_file(const Subject &subject,
                                     int page_num, int last_check) {
  json j;
  j["entries"] = json::array();
  j["current_page"] = page_num;
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = subject.get_db_id();
  std::clog << "query list....\n";
    std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(db_selected);

    std::string device_id = mysqlConn->escapeString(std::to_string(subject.get_device_id()));
    std::string username = mysqlConn->escapeString(subject.get_sub());
    std::string last_mod = mysqlConn->escapeString(std::to_string(last_check));
    std::string page_number = mysqlConn->escapeString(std::to_string(page_num * ENTRIES_PAGE));
    std::string limit_page_num = mysqlConn->escapeString(std::to_string((page_num + 1) * ENTRIES_PAGE));




  std::string query =
      "select t1.c_path, t1.num_chunks, t1.c_lastmod, t2.last_page, case when "
      "t1.check1 = "+device_id+" then 'deleted' else 'updated' end as command from (select "
       "distinct c_path, num_chunks, c_lastmod, device_id ^ "+device_id+" as check1 from "
      "chunks where c_username = '"+username+"' AND(device_id ^ "+device_id+") != 0 AND c_lastmod > "+last_mod+" "
      "limit "+page_number+", "+limit_page_num+") as t1, (select(ceil((count(distinct c_path) / "+std::to_string(ENTRIES_PAGE)+"))) -1 as "
      "last_page from chunks where c_username = '"+username+"' AND(device_id ^ "+device_id+") != 0 AND "
       "c_lastmod > "+last_mod+") as t2;";

    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};

    json j_single_path = {};

  if (res->rowsCount() > 0) {
    for (int i = 0; i < ENTRIES_PAGE; i++) {
      if (res->next()) {
        j_single_path["path"] = res->getString(1);
        j_single_path["num_chunks"] = res->getInt(2);
        j_single_path["last_mod"] = res->getInt(3);
        j_single_path["command"] = res->getString(5);
        j["entries"].push_back(j_single_path);
        j["last_page"] = res->getInt(4);
      } else
        break;
    }
  } else
    j["last_page"] = 0;
  return j;
}
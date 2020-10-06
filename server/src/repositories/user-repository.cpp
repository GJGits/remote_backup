#include "../../include/repositories/user-repository.hpp"

std::shared_ptr<UserRepository> UserRepository::getInstance() {
  if(instance.get() == nullptr) {
    instance = std::shared_ptr<UserRepository>{};
  }
  return instance;
}

size_t UserRepository::insertUser(const UserEntity &user) {
  std::clog << user.getUsername() << "\n";
  std::clog << user.getHashedPassword() << "\n";
  std::clog << user.getSalt() << "\n";
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->insertUsernameInDB(user.getUsername());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);

  stmt = std::unique_ptr<
      sql::PreparedStatement>{std::move(con->prepareStatement(
      "INSERT INTO users(username, hashed_password, salt, device_1_MAC,device_2_MAC,device_3_MAC,device_4_MAC,device_5_MAC,device_6_MAC,device_7_MAC,device_8_MAC) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))};

  stmt->setString(1, sql::SQLString{user.getUsername().c_str()});
  stmt->setString(2, sql::SQLString{user.getHashedPassword().c_str()});
  stmt->setUInt(3, user.getSalt());
  stmt->setString(4, sql::SQLString{user.get_device_1_MAC().c_str()});
    stmt->setString(5, sql::SQLString{user.get_device_2_MAC().c_str()});
    stmt->setString(6, sql::SQLString{user.get_device_3_MAC().c_str()});
    stmt->setString(7, sql::SQLString{user.get_device_4_MAC().c_str()});
    stmt->setString(8, sql::SQLString{user.get_device_5_MAC().c_str()});
    stmt->setString(9, sql::SQLString{user.get_device_6_MAC().c_str()});
    stmt->setString(10, sql::SQLString{user.get_device_7_MAC().c_str()});
    stmt->setString(11, sql::SQLString{user.get_device_8_MAC().c_str()});


    stmt->executeUpdate();
  return db_selected;
}

void UserRepository::updateUser(const UserEntity &user) {
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t db_selected = db_repinstance->getDBbyUsername(user.getUsername());
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement("update users set device_1_MAC = ?, device_2_MAC = ?, device_3_MAC = ?, device_4_MAC = ?, device_5_MAC = ?, device_6_MAC = ?, device_7_MAC = ?, device_8_MAC = ? where username = ?"))};
    stmt->setString(1, sql::SQLString{user.get_device_1_MAC().c_str()});
    stmt->setString(2, sql::SQLString{user.get_device_2_MAC().c_str()});
    stmt->setString(3, sql::SQLString{user.get_device_3_MAC().c_str()});
    stmt->setString(4, sql::SQLString{user.get_device_4_MAC().c_str()});
    stmt->setString(5, sql::SQLString{user.get_device_5_MAC().c_str()});
    stmt->setString(6, sql::SQLString{user.get_device_6_MAC().c_str()});
    stmt->setString(7, sql::SQLString{user.get_device_7_MAC().c_str()});
    stmt->setString(8, sql::SQLString{user.get_device_8_MAC().c_str()});
    stmt->setString(9, sql::SQLString{user.getUsername().c_str()});
    stmt->executeUpdate();
    return;
}

UserEntity UserRepository::getUserByUsername(const std::string &username) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(username);
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  stmt = std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement("SELECT username,hashed_password, salt, device_1_MAC,device_2_MAC,device_3_MAC,device_4_MAC,device_5_MAC,device_6_MAC,device_7_MAC,device_8_MAC FROM users WHERE username = ?"))};
  stmt->setString(1, sql::SQLString{username.c_str()});

    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};


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

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(username);
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  stmt = std::unique_ptr<sql::PreparedStatement>{
      std::move(con->prepareStatement("DELETE from users WHERE username = ?"))};
  stmt->setString(1, username);
  return stmt->executeUpdate() == 1 ? true : false;
}

std::string UserRepository::get_hashed_status(const std::string &username) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(username);
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  stmt = std::unique_ptr<
      sql::PreparedStatement>{std::move(con->prepareStatement(
      "SELECT c_hash FROM chunks WHERE c_username = ? ORDER BY c_path,c_id;"))};
  stmt->setString(1, sql::SQLString{username.c_str()});
  res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
  std::string hashed_status_concat;
  while (res->next()) {
    hashed_status_concat.append(std::move(res->getString("c_hash")));
  }
  std::vector<char> data(hashed_status_concat.begin(),
                         hashed_status_concat.end());
  return Sha256::getSha256(data);
}

json UserRepository::get_status_file(const UserEntity &user_entity) {
  json j;
  j["entries"] = json::array();
  j["current_page"] = user_entity.getpage_num();
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(user_entity.getUsername());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  stmt =std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement("select * from (select distinct c_path, num_chunks, c_lastmod from chunks where c_username=? limit ?, ?) as t1, (select (ceil((count(distinct c_path) / ?))) -1 as last_page from chunks where c_username=?) as t2;"))}; //ricordare al posto di 0, di mettere il vero valore
  stmt->setString(1, sql::SQLString{user_entity.getUsername().c_str()});
  stmt->setInt(2, (user_entity.getpage_num() * ENTRIES_PAGE));
  stmt->setInt(3, ((user_entity.getpage_num()+1) * ENTRIES_PAGE));
  stmt->setInt(4, ENTRIES_PAGE);
  stmt->setString(5, sql::SQLString{user_entity.getUsername().c_str()});
  res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
  json j_single_path = {};

  if(res->rowsCount() > 0) {
      for (int i = 0; i < ENTRIES_PAGE; i++) {
        if (res->next()) {
            j_single_path["path"] = res->getString("c_path");
            j_single_path["num_chunks"] = res->getInt("num_chunks");
            j_single_path["last_mod"] = res->getInt("c_lastmod");
            j["entries"].push_back(j_single_path);
            j["last_page"] = res->getInt("last_page");
        } else
            break;
    }
  }
  else
    j["last_page"] = 0;

  return j;
}
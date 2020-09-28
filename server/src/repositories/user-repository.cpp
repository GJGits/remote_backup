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
    std::clog << user.getDevice1() << "\n";
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->insertUsernameInDB(user.getUsername());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);

  stmt = std::unique_ptr<
      sql::PreparedStatement>{std::move(con->prepareStatement(
      "INSERT INTO users(username, hashed_password, salt, device1, device2, device3, numdevices) VALUES(?, ?, ?, ?, 'dev', 'dev', 1)"))};

  stmt->setString(1, sql::SQLString{user.getUsername().c_str()});
  stmt->setString(2, sql::SQLString{user.getHashedPassword().c_str()});
  stmt->setUInt(3, user.getSalt());
  stmt->setString(4, sql::SQLString{user.getDevice1().c_str()});

    stmt->executeUpdate();
  return db_selected;
}



UserEntity UserRepository::getUserByUsername(const UserEntity &user) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(user.getUsername());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
    std::clog << "La getUserByUsername\n";
  stmt = std::unique_ptr<sql::PreparedStatement>{
      std::move(con->prepareStatement("SELECT username,hashed_password, salt, device1, device2, device3, numdevices FROM users WHERE username = ?"))};
  stmt->setString(1, sql::SQLString{user.getUsername().c_str()});

    std::clog << "username: " <<user.getUsername() << "\n";

    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    std::clog << "username: " <<user.getUsername() << "\n";

    if (res->next()) {
    std::clog << "almeno entro\n";
    std::string hashed_password = std::move(res->getString("hashed_password"));
    unsigned int salt = res->getInt("salt");
    std::string device1 = std::move(res->getString("device1"));
      std::string device2 = std::move(res->getString("device2"));
      std::string device3 = std::move(res->getString("device3"));
      int numdevices = res->getInt("numdevices");
      std::clog << "username: " <<user.getUsername() << "\n";
      std::clog << "device1: " <<device1 << "\n";
      std::clog << "numdevices: " <<numdevices << "\n";
      UserEntity entity{user.getUsername(), hashed_password, salt, device1, device2, device3, numdevices};
    return entity;
  }
  throw UsernameNotExists();
}



void UserRepository::update_user(const UserEntity &user){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t db_selected = db_repinstance->getDBbyUsername(user.getUsername());
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
    stmt = std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement("UPDATE users set device2 = ? , device3 = ? ,numdevices = ? WHERE username = ?"))};

    std::clog << "Device2 id: "<<user.getDevice2() << "\n";
    std::clog << "Device3 id: "<<user.getDevice3() << "\n";
    std::clog << "Device nums: "<<user.getnumdevices() << "\n";
    std::clog << "Username: "<<user.getUsername() << "\n";

    stmt->setString(1, sql::SQLString{user.getDevice2().c_str()});
    stmt->setString(2, sql::SQLString{user.getDevice3().c_str()});
    stmt->setInt(3,user.getnumdevices()+1 );
    stmt->setString(4, sql::SQLString{user.getUsername().c_str()});

    stmt->executeUpdate();
    return;
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

json UserRepository::get_status_file(const std::string &username) {
  json j;
  j["entries"] = json::array();

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(username);
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  std::clog << "e0\n";
  stmt =
      std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
          "select * from chunks,fileinfo where chunks.c_username = ? and "
          "chunks.c_username = fileinfo.f_username and chunks.c_path = "
          "fileinfo.f_path order by fileinfo.f_path;"))};

  stmt->setString(1, sql::SQLString{username.c_str()});
  std::clog << "e1\n";

  res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
  std::clog << "e2\n";

  json j_entry;

  json j_chunks;
  j_chunks = json::array();

  json j_single_chunk;

  std::string actual_file_path{""};
  while (res->next()) {

    if (actual_file_path.compare(res->getString("f_path")) != 0) {
      if (actual_file_path.compare("") != 0) {
        j_entry["chunks"] = j_chunks;
        j["entries"].push_back(j_entry);
      }

      actual_file_path = res->getString("f_path");
      j_chunks.clear();

      j_entry["path"] = res->getString("f_path");
      j_entry["size"] = res->getInt("f_size");
      j_entry["validity"] = "false";
      j_entry["last_mod"] = res->getString("f_lastmod");
    }
    j_entry["dim_last_chunk"] = res->getInt("c_size");

    j_single_chunk["hash"] = res->getString("c_hash");
    j_single_chunk["id"] = res->getInt("c_id");
    j_chunks.push_back(j_single_chunk);
  }
  j_entry["chunks"] = j_chunks;
  j["entries"].push_back(j_entry);

  return j;
}
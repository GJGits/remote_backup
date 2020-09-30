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



UserEntity UserRepository::getUserByUsername(const std::string &username) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(username);
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
    std::clog << "La getUserByUsername\n";
  stmt = std::unique_ptr<sql::PreparedStatement>{
      std::move(con->prepareStatement("SELECT username,hashed_password, salt, device1, device2, device3, numdevices FROM users WHERE username = ?"))};
  stmt->setString(1, sql::SQLString{username.c_str()});

    std::clog << "username: " <<username << "\n";

    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};


    if (res->next()) {
    std::clog << "almeno entro\n";
    std::string hashed_password = std::move(res->getString("hashed_password"));
    unsigned int salt = res->getInt("salt");
    std::string device1 = std::move(res->getString("device1"));
      std::string device2 = std::move(res->getString("device2"));
      std::string device3 = std::move(res->getString("device3"));
      int numdevices = res->getInt("numdevices");
      std::clog << "username: " <<username << "\n";
      std::clog << "device1: " <<device1 << "\n";
      std::clog << "numdevices: " <<numdevices << "\n";
      UserEntity entity{username, hashed_password, salt, device1, device2, device3, numdevices};
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

json UserRepository::get_status_file(const UserEntity &user_entity) {
  json j;
  j["entries"] = json::array();
  j["current_page"] = user_entity.getpage_num();

  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(user_entity.getUsername());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  std::clog << "e0\n";

      stmt =
              std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
                      "select distinct(c_path),num_chunks,c_lastmod  from chunks where c_username = ? LIMIT ?,18446744073709551615;"))}; //ricordare al posto di 0, di mettere il vero valore
      std::clog << "username: " << user_entity.getUsername() << "\n";
      stmt->setString(1, sql::SQLString{user_entity.getUsername().c_str()});
      stmt->setInt(2, (user_entity.getpage_num() * 100));


    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};


    json j_single_path;
    int numRows = res->rowsCount() ;
    std::clog << "IL NUMERO DI RIGHE: " << numRows << "\n";
    for(int i=0 ; i<100 ; i++) {
        res->next();
        j_single_path["path"] = res->getString("c_path");
        j_single_path["num_chunks"] = res->getInt("num_chunks");
        j_single_path["last_mod"] = res->getInt("c_lastmod");
        j["entries"].push_back(j_single_path);

}

    int result = std::floor(numRows/100);
    int rest = numRows%100;
    if(rest == 0 && result > 0)
        result--;
  j["last_page"] = result;

  return j;
}
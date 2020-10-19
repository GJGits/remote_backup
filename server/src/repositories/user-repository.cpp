#include "../../include/repositories/user-repository.hpp"

std::shared_ptr<UserRepository> UserRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<UserRepository>{};
  }
  return instance;
}

size_t UserRepository::insertUser(const UserEntity &user) {


    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t db_selected = db_repinstance->insertUsernameInDB(user.getUsername());

    std::string query ="INSERT INTO users(username, hashed_password, salt, "
                       "device_1_MAC,device_2_MAC,device_3_MAC,device_4_MAC,device_5_MAC,"
                       "device_6_MAC,device_7_MAC,device_8_MAC) VALUES('?', '?', "+std::to_string(user.getSalt())+", '?', '?', '?', '?', '?', '?', '?', '?')";
    std::list<std::string> entries_of_query;
    entries_of_query.push_back(user.getUsername());
    entries_of_query.push_back(user.getHashedPassword());
    entries_of_query.push_back(user.get_device_1_MAC());
    entries_of_query.push_back(user.get_device_2_MAC());
    entries_of_query.push_back(user.get_device_3_MAC());
    entries_of_query.push_back(user.get_device_4_MAC());
    entries_of_query.push_back(user.get_device_5_MAC());
    entries_of_query.push_back(user.get_device_6_MAC());
    entries_of_query.push_back(user.get_device_7_MAC());
    entries_of_query.push_back(user.get_device_8_MAC());
    Utility::update_query(query,entries_of_query,db_selected);
    return db_selected;

}

bool UserRepository::UserAlreadyPresent(const std::string &username){

    std::unique_ptr<sql::ResultSet> res;

    std::string query = "Select username from UsersDB where username = '?';";
    std::list<std::string> entries_of_query;
    entries_of_query.push_back(username);

    res = Utility::select_query(query,entries_of_query,0);

    if (res->next()) {
        return true;
    }
    return false;
}

void UserRepository::updateUser(const UserEntity &user) {

    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t db_selected = db_repinstance->getDBbyUsername(user.getUsername());

    std::string query = "update users set device_1_MAC = '?', device_2_MAC = '?', device_3_MAC = '?', device_4_MAC = '?', device_5_MAC = '?', device_6_MAC = '?',device_7_MAC = '?', device_8_MAC = '?' where username = '?'";
    std::list<std::string> entries_of_query;
    entries_of_query.push_back(user.get_device_1_MAC());
    entries_of_query.push_back(user.get_device_2_MAC());
    entries_of_query.push_back(user.get_device_3_MAC());
    entries_of_query.push_back(user.get_device_4_MAC());
    entries_of_query.push_back(user.get_device_5_MAC());
    entries_of_query.push_back(user.get_device_6_MAC());
    entries_of_query.push_back(user.get_device_7_MAC());
    entries_of_query.push_back(user.get_device_8_MAC());
    entries_of_query.push_back(user.getUsername());
    Utility::update_query(query,entries_of_query,db_selected);
    return;
}

UserEntity UserRepository::getUserByUsername(const std::string &username) {

    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t db_selected = db_repinstance->getDBbyUsername(username);
    std::unique_ptr<sql::ResultSet> res;
    std::string query = "SELECT username,hashed_password, salt, "
                        "device_1_MAC,device_2_MAC,device_3_MAC,device_4_MAC,device_5_MAC,device_"
                        "6_MAC,device_7_MAC,device_8_MAC FROM users WHERE username = '?'";
    std::list<std::string> entries_of_query;
    entries_of_query.push_back(username);
    res = Utility::select_query(query,entries_of_query,db_selected);

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





json UserRepository::get_status_file(const Subject &subject,
                                     int page_num, int last_check) {

    size_t db_selected = subject.get_db_id();
    std::unique_ptr<sql::ResultSet> res;
    std::string query =
            "select t1.c_path, t1.num_chunks, t1.c_lastmod, t2.last_page, case when "
            "t1.check1 = "+std::to_string(subject.get_device_id())+" then 'deleted' else 'updated' end as command from (select "
            "distinct c_path, num_chunks, c_lastmod, device_id ^ "+std::to_string(subject.get_device_id())+" as check1 from "
            "chunks where c_username = '?' AND(device_id ^ "+std::to_string(subject.get_device_id())+") != 0 AND c_lastmod > "+std::to_string(last_check)+" "
            "limit "+std::to_string(page_num * ENTRIES_PAGE)+", "+std::to_string((page_num + 1) * ENTRIES_PAGE)+") as t1, (select(ceil((count(distinct c_path) / "+std::to_string(ENTRIES_PAGE)+"))) -1 as "
            "last_page from chunks where c_username = '?' AND(device_id ^ "+std::to_string(subject.get_device_id())+") != 0 AND "
             "c_lastmod > "+std::to_string(last_check)+") as t2;";

    std::list<std::string> entries_of_query;
    entries_of_query.push_back(subject.get_sub());
    entries_of_query.push_back(subject.get_sub());
    res = Utility::select_query(query,entries_of_query,db_selected);

  std::clog << "query list....\n";

    json j;
    j["entries"] = json::array();
    j["current_page"] = page_num;
    json j_single_path = {};

  if (res->rowsCount() > 0) {
    for (int i = 0; i < ENTRIES_PAGE; i++) {
      if (res->next()) {
        j_single_path["path"] = res->getString(1);
        j_single_path["num_chunks"] = res->getInt(2);
        j_single_path["last_remote_change"] = res->getInt(3);
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
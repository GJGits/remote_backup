#include "../../include/repositories/file-repository.hpp"

std::shared_ptr<FileRepository> FileRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<FileRepository>{};
  }
  return instance;
}

bool FileRepository::deleteFile(const FileEntity &file) {


  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(file.getUsername());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
    if (con->isValid() && !con->isClosed()) {
        int device_id=file.get_subject().get_device_id();
        std::string device_name="device" + std::to_string(device_id);

        stmt =
                std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
                        "SELECT device1,device2,device3 FROM chunks WHERE c_username = ? and c_path = ? LIMIT 1;"))};
        stmt->setString(1, sql::SQLString{file.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{file.getPathFile().c_str()});

        res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
        if (res->next()) {
            std::string my;
            std::string other1;
            std::string other2;
            if(device_name.compare("device1")==0){
                my=std::move(res->getString("device1"));
                other1=std::move(res->getString("device2"));
                other2=std::move(res->getString("device3"));
            }
            else if(device_name.compare("device2")==0){
                other1=std::move(res->getString("device1"));
                my=std::move(res->getString("device2"));
                other2=std::move(res->getString("device3"));
            }
            else{
                other1=std::move(res->getString("device1"));
                other2=std::move(res->getString("device2"));
                my=std::move(res->getString("device3"));
            }

            if((my.compare("GET")==0)||(my.compare("OK")==0)){
                stmt =
                        std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
                                "UPDATE chunks SET "+my + "='DELETE',"+other1+"='TODO-DELETE',"+other2+"='TODO-DELETE' WHERE c_username = ? and c_path = ?;"))};
                stmt->setString(1, sql::SQLString{file.getUsername().c_str()});
                stmt->setString(2, sql::SQLString{file.getPathFile().c_str()});
            }
            else if((other1.compare("DELETE")==0)&&(other2.compare("DELETE")==0)){
                stmt =
                        std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
                                "DELETE from chunks WHERE c_username = ? and c_path = ?;"))};
                stmt->setString(1, sql::SQLString{file.getUsername().c_str()});
                stmt->setString(2, sql::SQLString{file.getPathFile().c_str()});
            }
            else if(my.compare("TODO-DELETE")==0){
                stmt =
                        std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
                                "UPDATE chunks SET "+my + "='DELETE' from chunks WHERE c_username = ? and c_path = ?;"))};
                stmt->setString(1, sql::SQLString{file.getUsername().c_str()});
                stmt->setString(2, sql::SQLString{file.getPathFile().c_str()});
            }
        }

    return stmt->executeUpdate() == 1 ? true : false;
  }
  throw DatabaseInvalidConnection();
}
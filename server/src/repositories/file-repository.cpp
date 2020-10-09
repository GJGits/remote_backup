#include "../../include/repositories/file-repository.hpp"

std::shared_ptr<FileRepository> FileRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<FileRepository>{};
  }
  return instance;
}

bool FileRepository::deleteFile(const FileEntity &file) {

  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(file.getUsername());
    std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(db_selected);

    std::string username = mysqlConn->escapeString(file.getUsername());
    std::string path = mysqlConn->escapeString(file.getPathFile());

    std::clog << username << "\n";
    std::clog << path << "\n";

    std::string query = "DELETE from chunks WHERE c_username = '"+username+"' and c_path = '"+path+"';";

    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    stmt->execute(query);

    return true;

}
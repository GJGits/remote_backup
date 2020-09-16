#include "../../include/repositories/file-repository.hpp"
bool FileRepository::deleteFile(const FileEntity &file){

    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr <sql::Connection> con = DBConnect::getConnection();
    if(con->isValid() && !con->isClosed()) {
        stmt = std::unique_ptr < sql::PreparedStatement >
               {std::move(con->prepareStatement("DELETE FROM chunks WHERE c_username = ? and c_path = ?;"))};
        stmt->setString(1, sql::SQLString{file.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{file.getPathFile().c_str()});
        stmt->executeUpdate();
        return stmt->executeUpdate() == 1 ? true : false;
    }
    throw DatabaseInvalidConnection();
}
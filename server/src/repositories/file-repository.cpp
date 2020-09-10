#include "../../include/repositories/file-repository.hpp"

bool FileRepository::deleteFile(const FileEntity &file){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
        std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
        stmt = std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement("DELETE FROM fileinfo WHERE f_username = ? and f_path = ?;"))};
        stmt->setString(1, sql::SQLString{file.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{file.getPathFile().c_str()});
        if(stmt->executeUpdate() != 1){
            return false;
        }
        stmt = std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement("DELETE FROM chunks WHERE c_username = ? and c_path = ?;"))};
        stmt->setString(1, sql::SQLString{file.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{file.getPathFile().c_str()});
        stmt->executeUpdate();
        return true;
}
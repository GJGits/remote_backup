#include "../../include/repositories/file-repository.hpp"




bool FileRepository::deleteFile(const FileEntity &file){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;

    try {

        std::shared_ptr<sql::Connection> con = DBConnect::getConnection();

        stmt = std::unique_ptr<
               sql::PreparedStatement>{std::move(con->prepareStatement(
                "DELETE FROM fileinfo WHERE username = ? and path = ?;"))};

        stmt->setString(1, sql::SQLString{file.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{file.getPathFile().c_str()});

        if(stmt->executeUpdate() != 1){
            return false;
        }

        stmt = std::unique_ptr<
               sql::PreparedStatement>{std::move(con->prepareStatement(
        "DELETE FROM chunks WHERE username = ? and path = ?;"))};

        stmt->setString(1, sql::SQLString{file.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{file.getPathFile().c_str()});
        stmt->executeUpdate();
        return true;

    } catch (sql::SQLException &e) {
        //std::clog << "insert mysql error\n";
        Logger::log(std::string{"insert mysql error("} +
                    std::to_string(e.getErrorCode()) + std::string{")"});
        // ce la caviamo con una generica not
        return false;
    }

    return false;
}
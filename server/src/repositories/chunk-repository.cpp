#include "../../include/repositories/chunk-repository.hpp"

bool ChunkRepository::getFilePath(const ChunkEntity &chunk) {
    std::unique_ptr <sql::PreparedStatement> stmt;
    std::unique_ptr <sql::ResultSet> res;

    try {

        std::shared_ptr <sql::Connection> con = DBConnect::getConnection();
        stmt = std::unique_ptr < sql::PreparedStatement > {
                std::move(con->prepareStatement("SELECT username FROM fileinfo WHERE path = ?"))};
        stmt->setString(1, chunk.getPathFile());
        res = std::unique_ptr < sql::ResultSet > {std::move(stmt->executeQuery())};
        if (res->next()) {
            return true;
        } else {
            return false;
        }

        throw UsernameNotExists();

    } catch (sql::SQLException &e) {
        //std::clog << "select mysql error\n";
        Logger::log(std::string{"select mysql error("} +
                    std::to_string(e.getErrorCode()) + std::string{")"});
        // ce la caviamo con un generico not found
        throw UknownError();

    }

    throw UknownError();
}


bool ChunkRepository::addChunk(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;

    try {

        std::shared_ptr<sql::Connection> con = DBConnect::getConnection();

        stmt = std::unique_ptr<
               sql::PreparedStatement>{std::move(con->prepareStatement(
                "insert into chunks(username, id, hash, path, size) values(?,?,?,?,?);"))};

        stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setInt(2, chunk.getIdChunk());
        stmt->setString(3, sql::SQLString{chunk.getHashChunk().c_str()});
        stmt->setString(4, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setInt(5, chunk.getSizeChunk());



        return stmt->executeUpdate() == 1 ? true : false;

    } catch (sql::SQLException &e) {
        //std::clog << "insert mysql error\n";
        Logger::log(std::string{"insert mysql error("} +
                    std::to_string(e.getErrorCode()) + std::string{")"});
        // ce la caviamo con una generica not
        return false;
    }

    return false;
}


bool ChunkRepository::addFileInfo(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;

    try {

        std::shared_ptr<sql::Connection> con = DBConnect::getConnection();

        stmt = std::unique_ptr<
               sql::PreparedStatement>{std::move(con->prepareStatement(
                "insert into fileinfo(username, path, size, lastmod) values(?,?,?,?);"))};

        stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setInt(3, chunk.getSizeFile());
        stmt->setString(4, sql::SQLString{chunk.getLastMod().c_str()});


        return stmt->executeUpdate() == 1 ? true : false;

    } catch (sql::SQLException &e) {
        //std::clog << "insert mysql error\n";
        Logger::log(std::string{"insert mysql error("} +
                    std::to_string(e.getErrorCode()) + std::string{")"});
        // ce la caviamo con una generica not
        return false;
    }

    return false;
}

int ChunkRepository::getFileSize(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;

    try {

        std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
        stmt = std::unique_ptr<sql::PreparedStatement>{
                std::move(con->prepareStatement("SELECT size FROM fileinfo WHERE username = ? and path = ?"))};
        stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{chunk.getPathFile().c_str()});

        res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
        if (res->next()) {
            return res->getInt("size");

        }
        throw UsernameNotExists();

    } catch (sql::SQLException &e) {
        //std::clog << "select mysql error\n";
        Logger::log(std::string{"select mysql error("} +
                    std::to_string(e.getErrorCode()) + std::string{")"});
        // ce la caviamo con un generico not found
        throw UknownError();

    }

    throw UknownError();
}

bool ChunkRepository::updateFileInfo(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;

    try {

        std::shared_ptr<sql::Connection> con = DBConnect::getConnection();

        stmt = std::unique_ptr<
               sql::PreparedStatement>{std::move(con->prepareStatement(
                "UPDATE fileinfo SET size = ? , lastmod = ? WHERE path = ? AND username = ?;"))};

        stmt->setInt(1, chunk.getSizeFile());
        stmt->setString(2, sql::SQLString{chunk.getLastMod().c_str()});
        stmt->setString(3, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setString(4, sql::SQLString{chunk.getUsername().c_str()});


        return stmt->executeUpdate() == 1 ? true : false;

    } catch (sql::SQLException &e) {
        //std::clog << "insert mysql error\n";
        Logger::log(std::string{"insert mysql error("} +
                    std::to_string(e.getErrorCode()) + std::string{")"});
        // ce la caviamo con una generica not
        return false;
    }

    return false;
}


bool ChunkRepository::updateChunk(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;

    try {

        std::shared_ptr<sql::Connection> con = DBConnect::getConnection();

        stmt = std::unique_ptr<
               sql::PreparedStatement>{std::move(con->prepareStatement(
                "UPDATE chunks SET hash = ? , size = ? WHERE path = ? AND username = ? AND id = ?;"))};

        stmt->setString(1, sql::SQLString{chunk.getHashChunk().c_str()});
        stmt->setInt(2, chunk.getSizeChunk());
        stmt->setString(3, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setString(4, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setInt(5, chunk.getIdChunk());


        return stmt->executeUpdate() == 1 ? true : false;

    } catch (sql::SQLException &e) {
        //std::clog << "insert mysql error\n";
        Logger::log(std::string{"insert mysql error("} +
                    std::to_string(e.getErrorCode()) + std::string{")"});
        // ce la caviamo con una generica not
        return false;
    }

    return false;
}

bool ChunkRepository::delete_chunks(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;

    try {

        std::shared_ptr<sql::Connection> con = DBConnect::getConnection();

        stmt = std::unique_ptr<
               sql::PreparedStatement>{std::move(con->prepareStatement(
                "DELETE from chunks WHERE path = ? AND username = ? AND id >= ?;"))};

        stmt->setString(1, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setString(2, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setInt(3, chunk.getIdChunk());


        return stmt->executeUpdate() == 1 ? true : false;

    } catch (sql::SQLException &e) {
        //std::clog << "insert mysql error\n";
        Logger::log(std::string{"insert mysql error("} +
                    std::to_string(e.getErrorCode()) + std::string{")"});
        // ce la caviamo con una generica not
        return false;
    }

    return false;
}
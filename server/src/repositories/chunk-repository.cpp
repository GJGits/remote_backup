#include "../../include/repositories/chunk-repository.hpp"


bool ChunkRepository::getFilePath(const ChunkEntity &chunk) {
    std::unique_ptr <sql::PreparedStatement> stmt;
    std::unique_ptr <sql::ResultSet> res;
    std::shared_ptr <sql::Connection> con = DBConnect::getConnection();
    std::clog << "Il path file è: " << chunk.getPathFile() << "\n";
    std::clog << "Il username è: " << chunk.getUsername() << "\n";

    stmt = std::unique_ptr < sql::PreparedStatement > {
            std::move(con->prepareStatement("SELECT f_username FROM fileinfo WHERE f_path = ? and f_username = ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setString(2, sql::SQLString{chunk.getUsername().c_str()});
    res = std::unique_ptr < sql::ResultSet > {std::move(stmt->executeQuery())};
    return res->next() ?  true : false;
}


bool ChunkRepository::addChunk(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<
           sql::PreparedStatement>{std::move(con->prepareStatement(
            "insert into chunks(c_username, c_id, c_hash, c_path, c_size) values(?,?,?,?,?);"))};
    stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
    stmt->setInt(2, chunk.getIdChunk());
    stmt->setString(3, sql::SQLString{chunk.getHashChunk().c_str()});
    stmt->setString(4, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setInt(5, chunk.getSizeChunk());
    return stmt->executeUpdate() == 1 ? true : false;
}


bool ChunkRepository::addFileInfo(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<
           sql::PreparedStatement>{std::move(con->prepareStatement(
            "insert into fileinfo(f_username, f_path, f_size, f_lastmod) values(?,?,?,?);"))};
    stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
    stmt->setString(2, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setInt(3, chunk.getSizeFile());
    stmt->setString(4, sql::SQLString{chunk.getLastMod().c_str()});
    return stmt->executeUpdate() == 1 ? true : false;
}

int ChunkRepository::getFileSize(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{
            std::move(con->prepareStatement("SELECT f_size FROM fileinfo WHERE f_username = ? and f_path = ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
    stmt->setString(2, sql::SQLString{chunk.getPathFile().c_str()});

    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    if (res->next()) {
        return res->getInt("f_size");
    }
    throw FileSizeNotAvailable();
}

bool ChunkRepository::updateFileInfo(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<
       sql::PreparedStatement>{std::move(con->prepareStatement(
        "UPDATE fileinfo SET f_size = ? , f_lastmod = ? WHERE f_path = ? AND f_username = ?;"))};
    stmt->setInt(1, chunk.getSizeFile());
    stmt->setString(2, sql::SQLString{chunk.getLastMod().c_str()});
    stmt->setString(3, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setString(4, sql::SQLString{chunk.getUsername().c_str()});
    return stmt->executeUpdate() == 1 ? true : false;
}


bool ChunkRepository::updateChunk(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<
           sql::PreparedStatement>{std::move(con->prepareStatement(
            "UPDATE chunks SET c_hash = ? , c_size = ? WHERE c_path = ? AND c_username = ? AND c_id = ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getHashChunk().c_str()});
    stmt->setInt(2, chunk.getSizeChunk());
    stmt->setString(3, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setString(4, sql::SQLString{chunk.getUsername().c_str()});
    stmt->setInt(5, chunk.getIdChunk());
    return stmt->executeUpdate() == 1 ? true : false;
}

bool ChunkRepository::delete_chunks(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<
           sql::PreparedStatement>{std::move(con->prepareStatement(
            "DELETE from chunks WHERE c_path = ? AND c_username = ? AND c_id >= ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setString(2, sql::SQLString{chunk.getUsername().c_str()});
    stmt->setInt(3, chunk.getIdChunk());
    return stmt->executeUpdate() == 1 ? true : false;
}


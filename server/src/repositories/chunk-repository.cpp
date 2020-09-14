#include "../../include/repositories/chunk-repository.hpp"


bool ChunkRepository::getFilePath(const ChunkEntity &chunk) {

    std::unique_ptr <sql::PreparedStatement> stmt;
    std::unique_ptr <sql::ResultSet> res;
    std::shared_ptr <sql::Connection> con = DBConnect::getConnection();
    std::clog << "Il path file è: " << chunk.getPathFile() << "\n";
    std::clog << "Il username è: " << chunk.getUsername() << "\n";

    stmt = std::unique_ptr < sql::PreparedStatement > {std::move(con->prepareStatement("SELECT f_username FROM fileinfo WHERE f_path = ? and f_username = ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setString(2, sql::SQLString{chunk.getUsername().c_str()});
    res = std::unique_ptr < sql::ResultSet > {std::move(stmt->executeQuery())};
    return res->next() ?  true : false;
}


bool ChunkRepository::add_or_update_Chunk(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    if(con->isValid() && !con->isClosed()) {
        stmt = std::unique_ptr <
               sql::PreparedStatement > {std::move(con->prepareStatement(
                "INSERT INTO chunks(c_username, c_id, c_hash, c_path, c_size, c_data) values(?,?,?,?,?,?) ON DUPLICATE KEY UPDATE c_hash = ?, c_size = ? , c_data = ?;"))};
        stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setInt(2, chunk.getIdChunk());
        stmt->setString(3, sql::SQLString{chunk.getHashChunk().c_str()});
        stmt->setString(4, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setInt(5, chunk.getSizeChunk());

        std::istringstream chunk_body(std::string{chunk.getchunk_body().get()->begin(),chunk.getchunk_body().get()->end()});
        stmt->setBlob(6, &chunk_body);

        stmt->setString(7, sql::SQLString{chunk.getHashChunk().c_str()});
        stmt->setInt(8, chunk.getSizeChunk());
        stmt->setBlob(9, &chunk_body);
        return stmt->executeUpdate() == 1 ? true : false;
    }
    throw DatabaseInvalidConnection();
}


bool ChunkRepository::add_or_update_FileInfo(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    if(con->isValid() && !con->isClosed()) {

        stmt = std::unique_ptr<
               sql::PreparedStatement>{std::move(con->prepareStatement(
                "INSERT INTO fileinfo(f_username, f_path, f_size, f_lastmod) values(?,?,?,?) ON DUPLICATE KEY UPDATE f_size = ?, f_lastmod = ? ;"))};
        stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setInt(3, chunk.getSizeFile());
        stmt->setString(4, sql::SQLString{chunk.getLastMod().c_str()});
        stmt->setInt(5, chunk.getSizeFile());
        stmt->setString(6, sql::SQLString{chunk.getLastMod().c_str()});
    return stmt->executeUpdate() == 1 ? true : false;

    }
    throw DatabaseInvalidConnection();
}

int ChunkRepository::getFileSize(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    stmt = std::unique_ptr<sql::PreparedStatement>{
            std::move(con->prepareStatement("SELECT c_id, c_size FROM chunks WHERE c_username = ? and c_path = ? order by c_id desc;"))};
    stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
    stmt->setString(2, sql::SQLString{chunk.getPathFile().c_str()});

    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    if (res->next()) {
        return (res->getInt("c_id") * CHUNK_SIZE + res->getInt("c_size"));
    }
    throw FileSizeNotAvailable();
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


#include "../../include/repositories/chunk-repository.hpp"


bool ChunkRepository::add_or_update_Chunk(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    if(con->isValid() && !con->isClosed()) {
        stmt = std::unique_ptr <
               sql::PreparedStatement > {std::move(con->prepareStatement(
                "INSERT INTO chunks(c_username, c_id, c_hash, c_path, c_size,c_lastmod, c_data) values(?,?,?,?,?,?,?) ON DUPLICATE KEY UPDATE c_hash = ?, c_size = ? , c_data = ?, c_lastmod = ?;"))};
        stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setInt(2, chunk.getIdChunk());
        stmt->setString(3, sql::SQLString{chunk.getHashChunk().c_str()});
        stmt->setString(4, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setInt(5, chunk.getSizeChunk());
        stmt->setInt(6, chunk.getLastMod());

        std::istringstream chunk_body(std::string{chunk.getchunk_body().get()->begin(),chunk.getchunk_body().get()->end()});
        std::clog << chunk_body.str() << "\n";

        stmt->setBlob(7, &chunk_body);

        stmt->setString(8, sql::SQLString{chunk.getHashChunk().c_str()});
        stmt->setInt(9, chunk.getSizeChunk());
        stmt->setBlob(10, &chunk_body);
        stmt->setInt(11, chunk.getLastMod());

        return stmt->executeUpdate() == 1 ? true : false;
    }
    throw DatabaseInvalidConnection();
}



std::string ChunkRepository::get_Chunk(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    if(con->isValid() && !con->isClosed()) {
        std::clog << chunk.getUsername().c_str() << "\n";
        std::clog << chunk.getPathFile().c_str() << "\n";
        std::clog << chunk.getIdChunk()<< "\n";


        stmt = std::unique_ptr<sql::PreparedStatement>{
                std::move(con->prepareStatement(
                        "SELECT c_data,c_size FROM chunks WHERE c_username = ? and c_path = ? and c_id = ?;"))};
        stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setString(2, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setInt(3, chunk.getIdChunk());

        res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
        if (res->next()) {
            //int size = res->getInt("c_size");

            std::shared_ptr<char[]> buffer = std::shared_ptr<char[]>{new char[CHUNK_SIZE]};
            std::istream *blobData = res->getBlob("c_data");
            std::clog << blobData;
            //blobData->read(buffer.get(), size);

            //for(int i = 0 ; i< size ; i++){
              //  std::clog << buffer.get()[i];
            //}
            std::clog << "\n";
            return "ciao";

        }
        throw FileSizeNotAvailable();
    }
    throw DatabaseInvalidConnection();

}

bool ChunkRepository::delete_chunks(const ChunkEntity &chunk){
    std::unique_ptr<sql::PreparedStatement> stmt;
    std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::Connection> con = DBConnect::getConnection();
    if(con->isValid() && !con->isClosed()) {

        stmt = std::unique_ptr<
                sql::PreparedStatement>{std::move(con->prepareStatement(
                "DELETE from chunks WHERE c_path = ? AND c_username = ? AND c_id >= ?;"))};
        stmt->setString(1, sql::SQLString{chunk.getPathFile().c_str()});
        stmt->setString(2, sql::SQLString{chunk.getUsername().c_str()});
        stmt->setInt(3, chunk.getIdChunk());
        return stmt->executeUpdate() == 1 ? true : false;
    }
    throw DatabaseInvalidConnection();

}


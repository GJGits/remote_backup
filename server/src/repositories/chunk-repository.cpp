#include "../../include/repositories/chunk-repository.hpp"

bool ChunkRepository::add_or_update_Chunk(const ChunkEntity &chunk) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(chunk.getDb());

  if (con->isValid() && !con->isClosed()) {
    stmt =
        std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
            "INSERT INTO chunks(c_username, c_id, c_hash, c_path, "
            "c_size,c_lastmod) values(?,?,?,?,?,?) ON DUPLICATE KEY "
            "UPDATE c_hash = ?, c_size = ? , c_lastmod = ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
    stmt->setInt(2, chunk.getIdChunk());
    stmt->setString(3, sql::SQLString{chunk.getHashChunk().c_str()});
    stmt->setString(4, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setInt(5, chunk.getSizeChunk());
    stmt->setInt(6, chunk.getLastMod());
    stmt->setString(7, sql::SQLString{chunk.getHashChunk().c_str()});
    stmt->setInt(8, chunk.getSizeChunk());
    stmt->setInt(9, chunk.getLastMod());
    return stmt->executeUpdate() == 1 ? true : false;
  }
  throw DatabaseInvalidConnection();
}

std::string ChunkRepository::get_Chunk(const ChunkEntity &chunk) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(chunk.getUsername());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  if (con->isValid() && !con->isClosed()) {
    std::clog << chunk.getUsername().c_str() << "\n";
    std::clog << chunk.getPathFile().c_str() << "\n";
    std::clog << chunk.getIdChunk() << "\n";

    stmt = std::unique_ptr<sql::PreparedStatement>{std::move(
        con->prepareStatement("SELECT c_data,c_size FROM chunks WHERE "
                              "c_username = ? and c_path = ? and c_id = ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getUsername().c_str()});
    stmt->setString(2, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setInt(3, chunk.getIdChunk());

    res = std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery())};
    if (res->next()) {
      int size = res->getInt("c_size");
      std::shared_ptr<char[]> buffer =
          std::shared_ptr<char[]>{new char[size + 1]};
      memset(buffer.get(), '\0', size + 1);
      std::istream *blobData = res->getBlob("c_data");
      size_t read = 0;
      while (size > 0) {
        blobData->seekg(read);
        ssize_t to_read = size > 64 ? 64 : size;
        blobData->read(buffer.get() + read, to_read);
        size -= to_read;
        read += to_read;
      }

      for (size_t i = 0; i < read; i++) {
        std::clog << buffer[i];
      }
      std::clog << "\n";

      return "ciao";
    }
    throw FileSizeNotAvailable();
  }
  throw DatabaseInvalidConnection();
}

bool ChunkRepository::delete_chunks(const ChunkEntity &chunk) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(chunk.getUsername());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  if (con->isValid() && !con->isClosed()) {
    stmt = std::unique_ptr<sql::PreparedStatement>{
        std::move(con->prepareStatement("DELETE from chunks WHERE c_path = ? "
                                        "AND c_username = ? AND c_id >= ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setString(2, sql::SQLString{chunk.getUsername().c_str()});
    stmt->setInt(3, chunk.getIdChunk());
    return stmt->executeUpdate() == 1 ? true : false;
  }
  throw DatabaseInvalidConnection();
}

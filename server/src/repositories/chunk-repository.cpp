#include "../../include/repositories/chunk-repository.hpp"

std::shared_ptr<ChunkRepository> ChunkRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<ChunkRepository>{};
  }
  return instance;
}

bool ChunkRepository::add_or_update_Chunk(const ChunkEntity &chunk) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<sql::Connection> con =
      DBConnect::getConnection(chunk.get_subject().get_db_id());
  if (con->isValid() && !con->isClosed()) {
    stmt =
        std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
            "INSERT INTO chunks(c_username, c_id, c_hash, c_path, "
            "c_size,c_lastmod,num_chunks, device_id) values(?,?,?,?,?,?,?,?) "
            "ON DUPLICATE KEY "
            "UPDATE c_hash = ?, c_size = ? , c_lastmod = ?, device_id = "
            "device_id ^ ? ;"))};
    stmt->setString(1, sql::SQLString{chunk.get_subject().get_sub().c_str()});
    stmt->setInt(2, chunk.getIdChunk());
    stmt->setString(3, sql::SQLString{chunk.getHashChunk().c_str()});

    std::string delimiter{"../../filesystem/" + chunk.get_subject().get_sub() +
                          "/"};
    std::string s = chunk.getPathFile();
    stmt->setString(
        4, sql::SQLString{Utility::split_string(s, delimiter).c_str()});
    stmt->setInt(5, chunk.getSizeChunk());
    stmt->setInt(6, chunk.getLastMod());
    stmt->setInt(7, chunk.getNumChunks());
    stmt->setInt(8, (int)chunk.get_subject().get_device_id());
    stmt->setString(9, sql::SQLString{chunk.getHashChunk().c_str()});
    stmt->setInt(10, chunk.getSizeChunk());
    stmt->setInt(11, chunk.getLastMod());
    stmt->setInt(12, (int)chunk.get_subject().get_device_id());

    return stmt->executeUpdate() == 1 ? true : false;
  }
  throw DatabaseInvalidConnection();
}

bool ChunkRepository::delete_chunks(const ChunkEntity &chunk) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected =
      db_repinstance->getDBbyUsername(chunk.get_subject().get_sub());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  if (con->isValid() && !con->isClosed()) {

    stmt = std::unique_ptr<sql::PreparedStatement>{
        std::move(con->prepareStatement("UPDATE chunks SET device_id = 0 WHERE "
                                        "c_path = ? AND c_username = ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setString(2, sql::SQLString{chunk.get_subject().get_sub().c_str()});
    return stmt->executeUpdate() == 1 ? true : false;
  }
  throw DatabaseInvalidConnection();
}

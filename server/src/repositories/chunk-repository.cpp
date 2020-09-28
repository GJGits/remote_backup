#include "../../include/repositories/chunk-repository.hpp"

 std::shared_ptr<ChunkRepository> ChunkRepository::getInstance() {
  if(instance.get() == nullptr) {
    instance = std::shared_ptr<ChunkRepository>{};
  }
  return instance;
}

bool ChunkRepository::add_or_update_Chunk(const ChunkEntity &chunk) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(chunk.get_subject().get_db_id());

  if (con->isValid() && !con->isClosed()) {
      std::string device1;
      std::string device2;
      std::string device3;

      switch(chunk.get_subject().get_device_id()) {
          case 0 :
              device1 = "OK";
              device2 = "NONE";
              device3 = "NONE";
              break;

          case 1 :
              device1 = "NONE";
              device2 = "OK";
              device3 = "NONE";
              break;

          case 2 :
              device1 = "NONE";
              device2 = "NONE";
              device3 = "OK";
              break;

          default :
              break;
      }
      std::clog << "device numbers: "<< chunk.get_subject().get_device_id() << "\n";

      std::clog << "device1: "<< device1 << "\n";
    stmt =
        std::unique_ptr<sql::PreparedStatement>{std::move(con->prepareStatement(
            "INSERT INTO chunks(c_username, c_id, c_hash, c_path, "
            "c_size,c_lastmod, device1, device2, device3) values(?,?,?,?,?,?,?,?,?) ON DUPLICATE KEY "
            "UPDATE c_hash = ?, c_size = ? , c_lastmod = ?, device1 = ?, device2 = ?, device3 = ?;"))};
    stmt->setString(1, sql::SQLString{chunk.get_subject().get_sub().c_str()});
    stmt->setInt(2, chunk.getIdChunk());
    stmt->setString(3, sql::SQLString{chunk.getHashChunk().c_str()});
    stmt->setString(4, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setInt(5, chunk.getSizeChunk());
    stmt->setInt(6, chunk.getLastMod());
      stmt->setString(7, sql::SQLString{device1.c_str()});
      stmt->setString(8, sql::SQLString{device2.c_str()});
      stmt->setString(9, sql::SQLString{device3.c_str()});

      stmt->setString(10, sql::SQLString{chunk.getHashChunk().c_str()});
    stmt->setInt(11, chunk.getSizeChunk());
    stmt->setInt(12, chunk.getLastMod());

      stmt->setString(13, sql::SQLString{device1.c_str()});
      stmt->setString(14, sql::SQLString{device2.c_str()});
      stmt->setString(15, sql::SQLString{device3.c_str()});

    return stmt->executeUpdate() == 1 ? true : false;
  }
  throw DatabaseInvalidConnection();
}

bool ChunkRepository::get_Chunk(const ChunkEntity &chunk) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;

  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(chunk.get_subject().get_sub());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  if (con->isValid() && !con->isClosed()) {
    std::clog << chunk.get_subject().get_sub().c_str() << "\n";
    std::clog << chunk.getPathFile().c_str() << "\n";
    std::clog << chunk.getIdChunk() << "\n";

    int num_device = chunk.get_subject().get_device_id();

    stmt = std::unique_ptr<sql::PreparedStatement>{std::move(
        con->prepareStatement("UPDATE SET device"+std::to_string(num_device+1)+"='OK' FROM chunks WHERE c_username = ? and c_path = ? and c_id = ?;"))};
    stmt->setString(1, sql::SQLString{chunk.get_subject().get_sub().c_str()});
    stmt->setString(2, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setInt(3, chunk.getIdChunk());

    return stmt->executeUpdate() == 1 ? true : false;
  }
  throw DatabaseInvalidConnection();
}

bool ChunkRepository::delete_chunks(const ChunkEntity &chunk) {
  std::unique_ptr<sql::PreparedStatement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(chunk.get_subject().get_sub());
  std::shared_ptr<sql::Connection> con = DBConnect::getConnection(db_selected);
  if (con->isValid() && !con->isClosed()) {


    stmt = std::unique_ptr<sql::PreparedStatement>{
        std::move(con->prepareStatement("DELETE from chunks WHERE c_path = ? "
                                        "AND c_username = ? AND c_id >= ?;"))};
    stmt->setString(1, sql::SQLString{chunk.getPathFile().c_str()});
    stmt->setString(2, sql::SQLString{chunk.get_subject().get_sub().c_str()});
    stmt->setInt(3, chunk.getIdChunk());
    return stmt->executeUpdate() == 1 ? true : false;
  }
  throw DatabaseInvalidConnection();
}

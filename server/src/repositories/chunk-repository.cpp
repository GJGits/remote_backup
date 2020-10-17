#include "../../include/repositories/chunk-repository.hpp"

std::shared_ptr<ChunkRepository> ChunkRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<ChunkRepository>{};
  }
  return instance;
}

bool ChunkRepository::add_or_update_Chunk(const ChunkEntity &chunk) {
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
    std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(chunk.get_subject().get_db_id());

    std::string username_escaped = mysqlConn->escapeString(chunk.get_subject().get_sub());
    std::string c_id = mysqlConn->escapeString(std::to_string(chunk.getIdChunk()));
    std::string delimiter{"../../filesystem/" + chunk.get_subject().get_sub() +
                          "/"};
    std::string s = chunk.getPathFile();
    std::string c_path = mysqlConn->escapeString(Utility::split_string(s, delimiter));
    std::string c_size_chunks = mysqlConn->escapeString(std::to_string(chunk.getSizeChunk()));
    std::string c_last_mod = mysqlConn->escapeString(std::to_string(chunk.getLastMod()));
    std::string c_num_chunks = mysqlConn->escapeString(std::to_string(chunk.getNumChunks()));
    std::string c_device_id = mysqlConn->escapeString(std::to_string((int)chunk.get_subject().get_device_id()));

    std::string query =
            "INSERT INTO chunks(c_username, c_id, c_path, "
            "c_size,c_lastmod,num_chunks, device_id) values('"+username_escaped+"',"+c_id+",'"+c_path+"',"+c_size_chunks+","+c_last_mod+","+c_num_chunks+","+c_device_id+") "
            "ON DUPLICATE KEY "
            "UPDATE  c_size = "+c_size_chunks+" , c_lastmod = "+c_last_mod+", device_id = device_id ^ "+c_device_id+" ;";
    
    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    stmt->execute(query);

    return true ;

}

bool ChunkRepository::delete_chunks(const ChunkEntity &chunk) {
  std::unique_ptr<sql::Statement> stmt;
  std::unique_ptr<sql::ResultSet> res;
  std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
  size_t db_selected = db_repinstance->getDBbyUsername(chunk.get_subject().get_sub());
    std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(db_selected);

    std::string c_path = mysqlConn->escapeString(chunk.getPathFile());
    std::string username = mysqlConn->escapeString(chunk.get_subject().get_sub());

    std::string query = "UPDATE chunks SET device_id = 0 WHERE c_path = '"+c_path+"' AND c_username = '"+username+"';";

    stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
    stmt->execute(query);

    return true;

}

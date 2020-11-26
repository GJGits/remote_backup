#include "../../include/repositories/chunk-repository.hpp"

bool ChunkRepository::add_or_update_Chunk(const ChunkEntity &chunk) {


  std::unique_ptr<sql::ResultSet> res;
  	
    std::string query =
      "INSERT INTO chunks(c_username, c_id, c_path, "
      "c_lastmod,num_chunks, device_id) values('?'," +
      std::to_string(chunk.getIdChunk()) + ",'?',-1," +
      std::to_string(chunk.getNumChunks()) + "," +
      std::to_string((int)chunk.get_subject().get_device_id()) +
      ") "
      "ON DUPLICATE KEY "
      "UPDATE c_lastmod = -1, device_id = " +
      std::to_string((int)chunk.get_subject().get_device_id()) + ", num_chunks = "+std::to_string(chunk.getNumChunks()) + ";";
  std::list<std::string> entries_of_query;
  entries_of_query.push_back(chunk.get_subject().get_sub());
  entries_of_query.push_back(chunk.getFileName());

  update_query(query, entries_of_query, chunk.get_subject().get_db_id());
  
  if(chunk.getIdChunk() == chunk.getNumChunks()-1){
  	std::string query = "UPDATE chunks SET c_lastmod = "+std::to_string(std::time(nullptr))+" where c_username = '?' and c_path = '?';";
  
  std::list<std::string> entries_of_query;
  entries_of_query.push_back(chunk.get_subject().get_sub());
  entries_of_query.push_back(chunk.getFileName());
  update_query(query, entries_of_query, chunk.get_subject().get_db_id());
  
  }
  return true;

  
  throw MissingElementInDB();
}

#include "../../include/repositories/chunk-repository.hpp"

bool ChunkRepository::add_or_update_Chunk(const ChunkEntity &chunk) {
  std::string query =
      "INSERT INTO chunks(c_username, c_id, c_path, "
      "c_lastmod,num_chunks, device_id) values('?'," +
      std::to_string(chunk.getIdChunk()) + ",'?'," +
      std::to_string(chunk.getLastMod()) + "," +
      std::to_string(chunk.getNumChunks()) + "," +
      std::to_string((int)chunk.get_subject().get_device_id()) +
      ") "
      "ON DUPLICATE KEY "
      "UPDATE c_lastmod = " +
      std::to_string(chunk.getLastMod()) + ", device_id = " +
      std::to_string((int)chunk.get_subject().get_device_id()) + ", num_chunks = "+std::to_string(chunk.getNumChunks()) + ";";
  std::list<std::string> entries_of_query;
  entries_of_query.push_back(chunk.get_subject().get_sub());
  entries_of_query.push_back(chunk.getFileName());

  return update_query(query, entries_of_query, chunk.get_subject().get_db_id());
}

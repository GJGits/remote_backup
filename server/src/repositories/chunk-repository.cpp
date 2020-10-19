#include "../../include/repositories/chunk-repository.hpp"

std::shared_ptr<ChunkRepository> ChunkRepository::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<ChunkRepository>{};
  }
  return instance;
}

bool ChunkRepository::add_or_update_Chunk(const ChunkEntity &chunk) {

    std::shared_ptr<DBRepository> db_repinstance = DBRepository::getInstance();
    size_t db_selected = db_repinstance->getDBbyUsername(chunk.get_subject().get_sub());

    std::string query =
            "INSERT INTO chunks(c_username, c_id, c_path, "
            "c_lastmod,num_chunks, device_id) values('?',"+std::to_string(chunk.getIdChunk())+",'?',"+std::to_string(chunk.getLastMod())+","+std::to_string(chunk.getNumChunks())+","+std::to_string((int)chunk.get_subject().get_device_id())+") "
                                                                                                                                                                                                                                                                                               "ON DUPLICATE KEY ""UPDATE c_lastmod = "+std::to_string(chunk.getLastMod())+", device_id = device_id ^ "+std::to_string((int)chunk.get_subject().get_device_id())+" ;";
    std::list<std::string> entries_of_query;
    entries_of_query.push_back(chunk.get_subject().get_sub());
    std::string s = chunk.getPathFile();
    std::string delimiter{"../../filesystem/" + chunk.get_subject().get_sub() + "/"};
    entries_of_query.push_back(Utility::split_string(s, delimiter));



    return Utility::update_query(query,entries_of_query,db_selected);
}



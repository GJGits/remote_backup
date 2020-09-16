#include "../../include/services/chunk-service.hpp"

std::shared_ptr<ChunkService> ChunkService::getInstance() {
    if (instance.get() == nullptr) {
        instance = std::shared_ptr<ChunkService>(new ChunkService{});
    }
    return instance;
}

void ChunkService::file_chunk_add(const PostChunkDTO &post_chunk) {
    if(Sha256::getSha256(*post_chunk.getchunk_body()).compare(post_chunk.getchunk_hash()) == 0){
        ChunkEntity chunk_ent{post_chunk};
        ChunkRepository chunk_rep;
        chunk_rep.add_or_update_Chunk(chunk_ent);
        return;
    }
    throw ChunkCorrupted();
}

void ChunkService::file_chunk_update(const PutChunkDTO &put_chunk) {
    if(Sha256::getSha256(*put_chunk.getchunk_body()).compare(put_chunk.getchunk_hash()) == 0) {
        ChunkEntity chunk_ent{put_chunk};
        ChunkRepository chunk_rep;
        chunk_rep.add_or_update_Chunk(chunk_ent);
        return;
    }
    throw ChunkCorrupted();
}


std::string ChunkService::file_chunk_get(const GetChunkDTO &get_chunk) {
    ChunkEntity chunk_ent{get_chunk};
    ChunkRepository chunk_rep;
    return chunk_rep.get_Chunk(chunk_ent);
}

void ChunkService::file_chunk_delete_service(const DeleteChunkDTO &del_chunk){

    ChunkEntity chunk_ent{del_chunk};
    ChunkRepository chunk_rep;
    chunk_rep.delete_chunks(chunk_ent);
    return;
}

#include "../../include/services/chunk-service.hpp"

std::shared_ptr<ChunkService> ChunkService::getInstance() {
    if (instance.get() == nullptr) {
        instance = std::shared_ptr<ChunkService>(new ChunkService{});
    }
    return instance;
}

void ChunkService::file_chunk_add(const PostChunkDTO &post_chunk) {
    std::clog << "l'hash vero è: "<< Sha256::getSha256(*post_chunk.getchunk_body()) << "\n";
    if(Sha256::getSha256(*post_chunk.getchunk_body()).compare(post_chunk.getchunk_hash()) == 0){
        ChunkEntity chunk_ent{post_chunk};
        ChunkRepository chunk_rep;
        chunk_rep.add_or_update_Chunk(chunk_ent);
        chunk_ent.setSizeFile(chunk_rep.getFileSize(chunk_ent));
        chunk_rep.add_or_update_FileInfo(chunk_ent);
        return;
    }
    throw ChunkCorrupted();
}

void ChunkService::file_chunk_update(const PutChunkDTO &put_chunk) {
    std::clog << "l'hash vero è: "<< Sha256::getSha256(*put_chunk.getchunk_body()) << "\n";

    if(Sha256::getSha256(*put_chunk.getchunk_body()).compare(put_chunk.getchunk_hash()) == 0) {
        ChunkEntity chunk_ent{put_chunk};
        ChunkRepository chunk_rep;
        chunk_ent.setSizeFile(chunk_rep.getFileSize(chunk_ent));
        chunk_rep.add_or_update_Chunk(chunk_ent);
        chunk_rep.add_or_update_FileInfo(chunk_ent);
        return;
    }
    throw ChunkCorrupted();
}


std::string ChunkService::file_chunk_get(const GetChunkDTO &get_file) {
    return "nulla";
}

void ChunkService::file_chunk_delete_service(const DeleteChunkDTO &del_chunk){

    ChunkEntity chunk_ent{del_chunk};
    ChunkRepository chunk_rep;
    chunk_ent.setSizeFile(chunk_rep.getFileSize(chunk_ent));
    chunk_rep.delete_chunks(chunk_ent);
    chunk_rep.add_or_update_FileInfo(chunk_ent);
    return;
}

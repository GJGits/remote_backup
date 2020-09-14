#include "../../include/services/chunk-service.hpp"

std::shared_ptr<ChunkService> ChunkService::getInstance() {
    if (instance.get() == nullptr) {
        instance = std::shared_ptr<ChunkService>(new ChunkService{});
    }
    return instance;
}

int ChunkService::div_ceil(int numerator, int denominator)
{
    std::div_t res = std::div(numerator, denominator);
    return res.rem ? (res.quot + 1) : res.quot;
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

        chunk_ent.setSizeFile(4); //todo: calcolarla per bene
        chunk_rep.add_or_update_Chunk(chunk_ent);
        chunk_rep.add_or_update_FileInfo(chunk_ent);
        return;
    }
    throw ChunkCorrupted();
}


std::string ChunkService::file_chunk_get(const GetChunkDTO &get_file) {
    std::fstream infile;
    std::string full_path = get_file.get_full_file_path();
    open_read_file_custom(full_path, infile);
    return read_custom(get_file.getchunk_size(), get_file.getchunk_id(),infile);
}

void ChunkService::file_chunk_delete_service(const DeleteChunkDTO &del_chunk){

    ChunkEntity chunk_ent{del_chunk};
    ChunkRepository chunk_rep;
    chunk_ent.setSizeFile(4);
    chunk_rep.delete_chunks(chunk_ent);
    chunk_rep.add_or_update_FileInfo(chunk_ent);
    return;
}

#include "../../include/services/chunk-service.hpp"


int div_ceil(int numerator, int denominator)
{
    std::div_t res = std::div(numerator, denominator);
    return res.rem ? (res.quot + 1) : res.quot;
}


void ChunkService::file_chunk_add(const PostChunkDTO &post_file) {
    std::clog << "l'hash vero è: "<< Sha256::getSha256(post_file.getchunk_body()) << "\n";

    if(Sha256::getSha256(post_file.getchunk_body()).compare(post_file.getchunk_hash()) == 0){
        ChunkEntity chunk_ent{post_file.getusername(), post_file.getchunk_id(), post_file.getchunk_hash(), post_file.getfile_path(), post_file.getchunk_size(), post_file.gettimestamp_locale()};
        ChunkRepository chunk_rep;
        std::ofstream outfile;
        std::string path{"../../filesystem/"+post_file.getusername()+"/"+post_file.getfile_path()};
        open_file_custom(path,outfile);
        std::vector<char> chunk_body{post_file.getchunk_body()};
        if(!chunk_rep.getFilePath(chunk_ent)) {  /* File doesn't exist yet */
            chunk_ent.setSizeFile((post_file.getchunk_id())*CHUNK_SIZE + post_file.getchunk_size());
            fill_padding(get_index(),post_file.getchunk_id(),outfile);
            chunk_rep.addFileInfo(chunk_ent);
        }else { /*File exists*/
            if(post_file.getchunk_id() >= div_ceil(chunk_rep.getFileSize(chunk_ent),CHUNK_SIZE)){ /*Chunk received is higher than the maximum already received*/
                fill_padding(div_ceil(chunk_rep.getFileSize(chunk_ent),CHUNK_SIZE) ,post_file.getchunk_id(),outfile);
                chunk_ent.setSizeFile((post_file.getchunk_id())*CHUNK_SIZE + post_file.getchunk_size());
            }else /*Chunk received is lower than the maximum already received*/
                chunk_ent.setSizeFile(chunk_rep.getFileSize(chunk_ent));
            chunk_rep.updateFileInfo(chunk_ent);
        }
        outfile.seekp(post_file.getchunk_id()*CHUNK_SIZE, std::ios_base::beg);
        std::string strvec{chunk_body.begin(),chunk_body.end()};
        outfile.write(strvec.c_str(),strvec.size());
        outfile.close();
        chunk_rep.addChunk(chunk_ent);
        return;
    }

    throw ChunkCorrupted();

}

void ChunkService::file_chunk_update(const PutChunkDTO &put_file) {
    std::clog << "l'hash vero è: "<< Sha256::getSha256(put_file.getchunk_body()) << "\n";

    if(Sha256::getSha256(put_file.getchunk_body()).compare(put_file.getchunk_hash()) == 0) {

        ChunkEntity chunk_ent{put_file.getusername(), put_file.getchunk_id(), put_file.getchunk_hash(), put_file.getfile_path(), put_file.getchunk_size(),put_file.gettimestamp_locale()};
        ChunkRepository chunk_rep;
        std::ofstream outfile;
        outfile.open("../../filesystem/"+put_file.getusername()+"/"+put_file.getfile_path() , std::ios::binary);
        int max_id = div_ceil(chunk_rep.getFileSize(chunk_ent),CHUNK_SIZE);
        std::vector<char> chunk_body{put_file.getchunk_body()};
        outfile.seekp(put_file.getchunk_id()*CHUNK_SIZE, std::ios_base::beg);
        std::string strvec{chunk_body.begin(),chunk_body.end()};
        outfile.write(strvec.c_str(),strvec.size());
        chunk_rep.updateChunk(chunk_ent);
        int filesize = 0;
        if(max_id == put_file.getchunk_id()) {
            int dim = max_id * CHUNK_SIZE + put_file.getchunk_size();
            std::filesystem::resize_file("../../filesystem/"+put_file.getusername()+"/"+put_file.getfile_path(), dim);

            filesize = put_file.getchunk_id() *CHUNK_SIZE + put_file.getchunk_size();
        }
        else{
            filesize = chunk_rep.getFileSize(chunk_ent);
        }
        chunk_ent.setSizeFile(filesize);
        chunk_rep.updateFileInfo(chunk_ent);

        outfile.close();

        return;
    }
    throw ChunkCorrupted();


}


std::string ChunkService::file_chunk_get(const GetChunkDTO &get_file) {
    std::fstream file(("../../filesystem/"+get_file.getusername()+"/"+get_file.getfile_path()), std::ios::in );
    char F[get_file.getchunk_size()+1];
    file.seekg(get_file.getchunk_id()*CHUNK_SIZE, std::ios::beg);
    file.read(F, get_file.getchunk_size());
    F[get_file.getchunk_size()] = 0;
    std::string s(F);
    file.close();
    return s;
}

void ChunkService::file_chunk_delete_service(const DeleteChunkDTO &del_chunk){

    ChunkEntity chunk_ent{del_chunk.getusername(), del_chunk.getchunk_id(), del_chunk.getfile_path(), del_chunk.getchunk_size(),del_chunk.gettimestamp_locale()};
    ChunkRepository chunk_rep;
    chunk_rep.delete_chunks(chunk_ent);
    int filesize = del_chunk.getchunk_id() * CHUNK_SIZE;
    chunk_ent.setSizeFile(filesize);
    chunk_rep.updateFileInfo(chunk_ent);
    std::filesystem::resize_file("../../filesystem/"+del_chunk.getusername()+"/"+del_chunk.getfile_path(), filesize);
    return;
}
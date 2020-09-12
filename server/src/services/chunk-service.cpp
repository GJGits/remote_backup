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
    std::clog << "l'hash vero è: "<< Sha256::getSha256(post_chunk.getchunk_body()) << "\n";
    if(Sha256::getSha256(post_chunk.getchunk_body()).compare(post_chunk.getchunk_hash()) == 0){
        ChunkEntity chunk_ent{post_chunk.getusername(), post_chunk.getchunk_id(), post_chunk.getchunk_hash(), post_chunk.getfile_path(), post_chunk.getchunk_size(), post_chunk.gettimestamp_locale()};
        ChunkRepository chunk_rep;
        std::ofstream outfile;
        /*Un lock messo qui fa funzionare*/
        //std::unique_lock lk(mtx);

        if(!std::filesystem::exists(post_chunk.get_full_file_path())) {  /* File doesn't exist yet */

            std::string directories = Utility::directories_path(post_chunk.get_full_file_path(),'/');
            if(!directories.empty())
                std::filesystem::create_directories(directories);
            outfile.open(post_chunk.get_full_file_path(), std::ios::binary | std::ios::out);
            if(!outfile.is_open())
                throw FileNotOpened();
            fill_padding(0,post_chunk.getchunk_id(),outfile);
        }else { /*File exists*/
            outfile.open(post_chunk.get_full_file_path() , std::ios::binary | std::ios::in | std::ios::out);
            if(!outfile.is_open())
                throw FileNotOpened();
            size_t file_size = std::filesystem::file_size(post_chunk.get_full_file_path());
            if(post_chunk.getchunk_id() >= div_ceil(file_size,CHUNK_SIZE)) /*Chunk received is higher than the maximum already received*/
                fill_padding(div_ceil(file_size,CHUNK_SIZE) ,post_chunk.getchunk_id(),outfile);
        }
        write_data_in_file_close(post_chunk.getchunk_id(), post_chunk.getchunk_body(), outfile);
        chunk_rep.addChunk(chunk_ent);
        chunk_ent.setSizeFile(std::filesystem::file_size(post_chunk.get_full_file_path()));
        chunk_rep.add_or_update_FileInfo(chunk_ent);
        return;
    }
    throw ChunkCorrupted();
}

void ChunkService::file_chunk_update(const PutChunkDTO &put_chunk) {
    std::clog << "l'hash vero è: "<< Sha256::getSha256(put_chunk.getchunk_body()) << "\n";

    if(Sha256::getSha256(put_chunk.getchunk_body()).compare(put_chunk.getchunk_hash()) == 0) {
        ChunkEntity chunk_ent{put_chunk.getusername(), put_chunk.getchunk_id(), put_chunk.getchunk_hash(), put_chunk.getfile_path(), put_chunk.getchunk_size(),put_chunk.gettimestamp_locale()};
        ChunkRepository chunk_rep;
        std::ofstream outfile;
        std::string full_path = put_chunk.get_full_file_path();

        outfile.open(put_chunk.get_full_file_path() , std::ios::binary | std::ios::in | std::ios::out);
        if(!outfile.is_open())
            throw FileNotOpened();

        size_t file_size = std::filesystem::file_size(put_chunk.get_full_file_path());
        write_data_in_file_close(put_chunk.getchunk_id()*CHUNK_SIZE,put_chunk.getchunk_body(),outfile);
        if(div_ceil(file_size,CHUNK_SIZE) == put_chunk.getchunk_id()) { /* Chunk received is the maximum */
            std::filesystem::resize_file(full_path, div_ceil(file_size,CHUNK_SIZE) * CHUNK_SIZE + put_chunk.getchunk_size());
            chunk_ent.setSizeFile(put_chunk.getchunk_id() *CHUNK_SIZE + put_chunk.getchunk_size());
        }
        else /* Chunk received is lower than the maximum */
            chunk_ent.setSizeFile(file_size);
        chunk_rep.updateChunk(chunk_ent);
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

    ChunkEntity chunk_ent{del_chunk.getusername(), del_chunk.getchunk_id(), del_chunk.getfile_path(),del_chunk.gettimestamp_locale()};
    ChunkRepository chunk_rep;
    std::clog << del_chunk.get_full_file_path() << "\n";
    std::filesystem::resize_file(del_chunk.get_full_file_path(), del_chunk.getchunk_id() * CHUNK_SIZE);
    chunk_ent.setSizeFile(del_chunk.getchunk_id() * CHUNK_SIZE);
    chunk_rep.delete_chunks(chunk_ent);
    chunk_rep.add_or_update_FileInfo(chunk_ent);
    return;
}

void ChunkService::fill_padding(int start_index, int id_chunk, std::ofstream &outfile){
    std::string padding(CHUNK_SIZE, '\0');
    for(int i = start_index ; i < id_chunk ; i++){
        outfile.seekp(i*CHUNK_SIZE, std::ios_base::beg);
        outfile.write(padding.c_str(),CHUNK_SIZE);
    }
}


void ChunkService::open_read_file_custom(std::string &path, std::fstream &infile){
    if(std::filesystem::exists(path))
        infile.open(path, std::ios::binary | std::ios::in );
    if(!infile.is_open())
        throw FileNotOpened();
}

std::string ChunkService::read_custom(int size, int index, std::fstream &infile){
    char F[size+1];
    infile.seekg(index*CHUNK_SIZE, std::ios::beg);
    infile.read(F, size);
    F[size] = 0;
    std::string s(F);
    infile.close();
    return s;
}

void ChunkService::write_data_in_file_close(int index, std::vector<char> chunk_body, std::ofstream &outfile){
    outfile.seekp(index*CHUNK_SIZE, std::ios_base::beg);
    std::string strvec{chunk_body.begin(),chunk_body.end()};
    outfile.write(strvec.c_str(),strvec.size());
    outfile.close();
}
#pragma once
#include <string>
#include "../common/logger.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/put_chunk_dto.hpp"
#include "../dtos/delete_chunk_dto.hpp"

class ChunkEntity {
private:
    std::string username;
    int id_chunk;
    std::string hash_chunk;
    std::string path_file;
    int size_chunk;
    std::string last_mod;
    int size_file;
    std::shared_ptr<std::vector<char>> chunk_body{new std::vector<char>{}};

public:
    ChunkEntity(const PostChunkDTO &post_chunk):username{post_chunk.getusername()}, id_chunk{post_chunk.getchunk_id()}, hash_chunk{post_chunk.getchunk_hash()},
    path_file{post_chunk.getfile_path()},size_chunk{post_chunk.getchunk_size()},
    last_mod{post_chunk.gettimestamp_locale()}{
        std::move(post_chunk.getchunk_body().get()->begin(), post_chunk.getchunk_body().get()->end(),std::back_inserter(*chunk_body));
    };

    ChunkEntity(const PutChunkDTO &put_chunk):username(put_chunk.getusername()), id_chunk(put_chunk.getchunk_id()),
    hash_chunk(put_chunk.getchunk_hash()),path_file(put_chunk.getfile_path()),
    size_chunk(put_chunk.getchunk_size()),last_mod(put_chunk.gettimestamp_locale()){
        std::move(put_chunk.getchunk_body().get()->begin(), put_chunk.getchunk_body().get()->end(),std::back_inserter(*chunk_body));
    };

    ChunkEntity(const DeleteChunkDTO &del_chunk):username{std::move(del_chunk.getusername())}, id_chunk{del_chunk.getchunk_id()}, path_file{std::move(del_chunk.getfile_path())}, last_mod{std::move(del_chunk.gettimestamp_locale())}{};


    std::string getUsername() const{ return username; };
    int getIdChunk() const{return id_chunk;};
    std::string getHashChunk() const{return hash_chunk;};
    std::string getPathFile() const{return path_file;};
    int getSizeChunk() const{return size_chunk;};
    std::string getLastMod() const{return last_mod;};
    int getSizeFile() const{return size_file;};
    void setSizeFile(int val)  {size_file = val;};
    std::shared_ptr<std::vector<char>> getchunk_body() const { return chunk_body; }

};
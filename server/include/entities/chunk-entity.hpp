#pragma once
#include <string>
#include "../common/logger.hpp"

class ChunkEntity {
private:
    std::string username;
    int id_chunk;
    std::string hash_chunk;
    std::string path_file;
    int size_chunk;
    std::string last_mod;
    int size_file;
public:
    ChunkEntity(std::string username, int id_chunk, std::string hash_chunk, std::string path_file, int size_chunk, std::string last_mod, int size_file):
    username{std::move(username)}, id_chunk{id_chunk}, hash_chunk{std::move(hash_chunk)}, path_file{std::move(path_file)}, size_chunk{size_chunk}, last_mod{std::move(last_mod)}, size_file{size_file}{};

    std::string getUsername() const{ return username; };
    int getIdChunk() const{return id_chunk;};
    std::string getHashChunk() const{return hash_chunk;};
    std::string getPathFile() const{return path_file;};
    int getSizeChunk() const{return size_chunk;};
    std::string getLastMod() const{return last_mod;};
    int getSizeFile() const{return size_file;};


};
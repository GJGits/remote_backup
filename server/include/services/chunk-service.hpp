#pragma once
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/put_chunk_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/delete_chunk_dto.hpp"
#include "../common/sha256.hpp"
#include "../repositories/chunk-repository.hpp"
#include "../entities/chunk-entity.hpp"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "../exceptions/exceptions.hpp"
#include "../common/jwt.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ctime>
#include "../common/constants.hpp"


class ChunkService {
private:
    inline static ChunkService *instance = nullptr;
    int index = 0;
public:


    static ChunkService *getInstance() {
        if (instance == nullptr) {
            instance = new ChunkService();
            instance->set_index(0);
        }
        return instance;
    }
    void file_chunk_add(const PostChunkDTO &post_file);
    void file_chunk_update(const PutChunkDTO &put_file);
    std::string file_chunk_get(const GetChunkDTO &get_file);
    void file_chunk_delete_service(const DeleteChunkDTO &del_chunk);

    int get_index(){ return index;};
    void set_index(int val){ index = val; };


    void fill_padding(int start_index, int id_chunk, std::ofstream &outfile){
        std::string padding(CHUNK_SIZE, '0');
        for(int i = start_index ; i < id_chunk ; i++){
            std::clog << start_index << "\n";
                outfile.seekp(i*CHUNK_SIZE, std::ios_base::beg);
                outfile.write(padding.c_str(),CHUNK_SIZE);
        }
    }

    void open_file_custom(std::string &path, std::ofstream &outfile){
        if(std::filesystem::exists(path))
            outfile.open(path , std::ios::binary | std::ios::in | std::ios::out);
        else
            outfile.open(path, std::ios::binary | std::ios::out);
        if(!outfile.is_open())
            throw FileNotOpened();
    }

};

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
#include "../common/utility.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ctime>
#include "../common/constants.hpp"


class ChunkService {
private:
    inline static ChunkService *instance = nullptr;
public:


    static ChunkService *getInstance() {
        if (instance == nullptr) {
            instance = new ChunkService();
        }
        return instance;
    }
    int div_ceil(int numerator, int denominator);
    void file_chunk_add(const PostChunkDTO &post_chunk);
    void file_chunk_update(const PutChunkDTO &put_chunk);
    std::string file_chunk_get(const GetChunkDTO &get_file);
    void file_chunk_delete_service(const DeleteChunkDTO &del_chunk);
    void fill_padding(int start_index, int id_chunk, std::ofstream &outfile);
    void open_write_file_custom(std::string &path, std::ofstream &outfile);
    void open_read_file_custom(std::string &path, std::fstream &infile);
    void write_data_in_file_close(int index, std::vector<char> chunk_body, std::ofstream &outfile);
    std::string read_custom(int size,int index, std::fstream &infile);
};

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



class UserService {
private:
    inline static UserService *instance = nullptr;

public:


    static UserService *getInstance() {
        if (instance == nullptr) {
            instance = new UserService();
        }
        return instance;
    }


    std::string getStatus(const std::string &username);
    std::string file_chunk_add(const PostChunkDTO &post_file);
    std::string file_chunk_update(const PutChunkDTO &put_file);
    std::string file_chunk_get(const GetChunkDTO &get_file);
    std::string file_chunk_delete_service(const DeleteChunkDTO &del_chunk);
};

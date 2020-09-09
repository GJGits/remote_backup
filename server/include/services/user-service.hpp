#pragma once
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/put_chunk_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/delete_file_dto.hpp"
#include "../dtos/delete_chunk_dto.hpp"

#include "../common/sha256.hpp"
#include "../filesystem/client_struct.hpp"
#include "../repositories/user-repository.hpp"
#include "../repositories/chunk-repository.hpp"
#include "../repositories/file-repository.hpp"
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

#define CHUNK_SIZE 4096


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

    std::string login(const SigninDTO &user);
    std::string signup(const SignupDTO &user);
    std::string getStatus(const std::string &username);
    std::string getStatusFile(const std::string &username);
    std::string file_chunk_add(const PostChunkDTO &post_file);
    std::string file_chunk_update(const PutChunkDTO &put_file);
    std::string file_chunk_get(const GetChunkDTO &get_file);
    std::string delete_file_service(const DeleteFileDTO &del_file);
    std::string file_chunk_delete_service(const DeleteChunkDTO &del_chunk);

};

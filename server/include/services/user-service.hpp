#pragma once
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/post_file_dto.hpp"
#include "../dtos/put_file_dto.hpp"
#include "../dtos/get_file_dto.hpp"
#include "../common/sha256.hpp"
#include "../repositories/user-repository.hpp"
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

    std::string login(const SigninDTO &user);
    std::string signup(const SignupDTO &user);
    std::string getStatus(const std::string &username);
    std::string getStatusFile(const std::string &username);
    std::string file_chunk_add(const PostFileDTO &post_file);
    std::string file_chunk_update(const PutFileDTO &put_file);
    std::string file_chunk_get(const GetFileDTO &get_file);

};

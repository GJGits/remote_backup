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
#include "../repositories/db-repository.hpp"
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


class UserService {
    private:
    static inline std::shared_ptr<UserService> instance{nullptr};

public:


    static std::shared_ptr<UserService> getInstance();


    std::string login(const SigninDTO &user);
    std::string signup(const SignupDTO &user);
    std::string getStatus(const std::string &username);
    ~UserService() {}

};

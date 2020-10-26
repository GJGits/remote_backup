#pragma once
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/delete_file_dto.hpp"
#include "../common/sha256.hpp"
#include "../repositories/user-repository.hpp"
#include "../repositories/chunk-repository.hpp"
#include "../repositories/file-repository.hpp"
#include "../repositories/db-repository.hpp"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "../exceptions/exceptions.hpp"
#include "../common/jwt.hpp"
#include <ctime>
#include "../common/constants.hpp"


class UserService {
    private:
    static inline std::shared_ptr<UserService> instance{nullptr};
    std::shared_ptr<UserRepository> user_repository;
public:


    static std::shared_ptr<UserService> getInstance();


    std::string login(const SigninDTO &user);
    std::string signup(const SignupDTO &user);
    ~UserService() {}

};

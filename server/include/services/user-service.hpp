#pragma once
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../common/sha256.hpp"
#include "../repositories/user-repository.hpp"
#include "../repositories/db-repository.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/jwt.hpp"


class UserService: public Singleton<UserService> {
    private:
    friend class Singleton;
    UserService(){}

public:
    std::string login(const SigninDTO &user);
    std::string signup(const SignupDTO &user);
    ~UserService() {}

};

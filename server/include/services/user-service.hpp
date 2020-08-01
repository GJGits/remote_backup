#pragma once
#include "../dtos/user-log-dto.hpp"
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

    std::string login(const UserLogDTO &user);
    std::string signup(const UserLogDTO &user);
    std::string getStatus(const UserLogDTO &user);

    };

#include "../dtos/user-log-dto.hpp"
#include "../common/sha256.hpp"
#include "../../include/repositories/user-repository.hpp"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "../../include/error_message/error-message.hpp"

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

    std::tuple<error_enum,std::string> login(const UserLogDTO &user);
    std::tuple<error_enum,std::string> signup(const UserLogDTO &user);
    std::tuple<error_enum,std::string> getStatus(const UserLogDTO &user);

    };

#include "../dtos/user-log-dto.hpp"
#include "../common/sha256.hpp"
#include "../../include/repositories/user-repository.hpp"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>

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
    
    bool login(const UserLogDTO &user);
    bool signup(const UserLogDTO &user);

    };

#include "../dtos/user-log-dto.hpp"

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
};
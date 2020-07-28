
#include "../../include/services/user-service.hpp"
#include "../../include/repositories/user-repository.hpp"

bool UserService::login(const UserLogDTO &user) {
    UserRepository user_rep;
    std::optional<UserEntity> user_returned = user_rep.getUserByUsername( user.getUsername() );
    if(user_returned.has_value()){
        if(user_returned.value().getHashedPassword().compare(user.getPassword()) == 0)
            return true;
        else
            return false;
    }
    else
        return false;

    return false;
}


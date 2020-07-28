
#include "../../include/services/user-service.hpp"
#include "../../include/repositories/user-repository.hpp"
#include <cstdlib>

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

bool UserService::signup(const UserLogDTO &user) {

    /* Eventualmente tale check se deve essere fatto più avanti lo si sposta*/
    if(user.getPassword().compare(user.getPasswordConfirm()) != 0)
        return false;

    UserRepository user_rep;

    std::string username(user.getUsername());

    /* Se */

    unsigned int salt = rand() % 64;

    /* Qui bisogna creare e inserire la hashed_passwor */
    std::string hashedPassword(Sha256::getSha256(user.getPassword()));
    /*                                                */

    UserEntity user_to_insert{username, hashedPassword, salt};

    if(user_rep.insertUser( user_to_insert ))
        return true;
    else
        return false;

}


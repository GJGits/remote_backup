
#include "../../include/services/user-service.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>


bool UserService::login(const UserLogDTO &user) {
    UserRepository user_rep;
    std::optional<UserEntity> user_returned = user_rep.getUserByUsername( user.getUsername() );
    if(user_returned.has_value()){
        unsigned int salt = user_returned.value().getSalt();
        std::string hashed_password{Sha256::getSha256(std::to_string(salt) + user.getPassword() + std::to_string(salt))};
        if(user_returned.value().getHashedPassword().compare(hashed_password) == 0)
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
    unsigned int salt = abs(rand()) ;
    std::string hashedPassword{Sha256::getSha256(std::to_string(salt) + user.getPassword() + std::to_string(salt))};
    UserEntity user_to_insert{username, hashedPassword, salt};
    if(user_rep.insertUser( user_to_insert )) {
        /* Create user directory */
        std::string path{"../../filesystem/"+username};
        std::filesystem::create_directories(path);

        /* Create server-struct.json file */
        std::string file_path{path+"/server-struct.json"};
        std::ofstream ofs(file_path);
        boost::property_tree::ptree pt;
        pt.put("hashed_status","empty_hashed_status");
        boost::property_tree::ptree pt_under;
        pt.add_child("entries",pt_under);
        boost::property_tree::write_json(file_path,pt);
        ofs.close();

        return true;
    }
    else
        return false;

}


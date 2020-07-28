#include <string>
#pragma once

class UserLogDTO {
private:


    std::string username;
    std::string password;
    std::string password_confirm;

public:

    UserLogDTO(std::string username, std::string password);
    UserLogDTO(std::string username, std::string password, std::string password_confirm);

    std::string getUsername() const;
    std::string getPassword() const;
    std::string getPasswordConfirm() const;
};

#include "../../include/dtos/user-log-dto.hpp"

UserLogDTO::UserLogDTO(std::string username): username{std::move(username)}{}

UserLogDTO::UserLogDTO(std::string username, std::string password): username{std::move(username)}, password{std::move(password)} {}

UserLogDTO::UserLogDTO(std::string username, std::string password, std::string password_confirm): username{std::move(username)}, password{std::move(password)}, password_confirm{std::move(password_confirm)} {}

 std::string UserLogDTO::getUsername() const{
    return username;
}

std::string UserLogDTO::getPassword() const{
    return password;
}

std::string UserLogDTO::getPasswordConfirm() const{
    return password_confirm;
}
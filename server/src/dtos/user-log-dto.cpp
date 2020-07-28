#include "../../include/dtos/user-log-dto.hpp"

UserLogDTO::UserLogDTO(std::string username, std::string password): username{std::move(username)}, password{std::move(password)} {}

UserLogDTO::UserLogDTO(std::string username, std::string password, std::string password_confirm): username{std::move(username)}, password{std::move(password)}, password_confirm{std::move(password_confirm)} {}

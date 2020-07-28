#include "../../include/dtos/user-log-dto.hpp"

UserLogDTO::UserLogDTO(std::string username, std::string password): username{std::move(username)}, password{std::move(password)} {}


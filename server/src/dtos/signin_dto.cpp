#include "../../include/dtos/signin_dto.hpp"


std::string SigninDTO::getUsername() const { return username; }

std::string SigninDTO::getPassword() const { return password; }

std::string SigninDTO::getMAC() const { return mac_address; }

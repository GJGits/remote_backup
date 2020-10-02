#include "../../include/dtos/signup_dto.hpp"

std::string SignupDTO::getUsername() const { return username; }

std::string SignupDTO::getPassword() const { return password; }

std::string SignupDTO::getPasswordConfirm() const { return password_confirm; }

std::string SignupDTO::getMAC() const { return mac_address; }
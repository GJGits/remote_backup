#include "../../include/dtos/signup_dto.hpp"

SignupDTO::SignupDTO(std::string username, std::string password,
                     std::string password_confirm)
    : username{std::move(username)}, password{std::move(password)},
      password_confirm{std::move(password_confirm)} {}

std::string SignupDTO::getUsername() const { return username; }

std::string SignupDTO::getPassword() const { return password; }

std::string SignupDTO::getPasswordConfirm() const { return password_confirm; }
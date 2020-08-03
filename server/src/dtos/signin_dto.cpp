#include "../../include/dtos/signin_dto.hpp"

SigninDTO::SigninDTO(const std::string &username, const std::string &password)
    : username{std::move(username)}, password{std::move(password)} {}

std::string SigninDTO::getUsername() const { return username; }

std::string SigninDTO::getPassword() const { return password; }
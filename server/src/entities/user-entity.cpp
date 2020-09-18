#include "../../include/entities/user-entity.hpp"

UserEntity::UserEntity(const std::string &username,
                       const std::string &hashed_password, uint32_t salt)
    : username{username}, hashed_password{hashed_password}, salt{salt} {}

std::string UserEntity::getUsername() const { return username; }
std::string UserEntity::getHashedPassword() const { return hashed_password; }
uint32_t UserEntity::getSalt() const { return salt; }

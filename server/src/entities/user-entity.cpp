#include "../lib/entities/user-entity.hpp"

UserEntity::UserEntity(std::string username,
                       std::string hashed_password, uint32_t salt)
    : username{std::move(username)},
      hashed_password{std::move(hashed_password)}, salt{salt} {}

std::string UserEntity::getUsername() const { return username; }
std::string UserEntity::getHashedPassword() const { return hashed_password; }
uint32_t UserEntity::getSalt() const { return salt; }
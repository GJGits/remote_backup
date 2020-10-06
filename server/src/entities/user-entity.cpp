#include "../../include/entities/user-entity.hpp"


UserEntity::UserEntity(const std::string &username,
                       const std::string &hashed_password, uint32_t salt)
        : username{username}, hashed_password{hashed_password}, salt{salt},
          device_1_MAC{""},device_2_MAC{""},device_3_MAC{""},
          device_4_MAC{""},device_5_MAC{""},device_6_MAC{""},
          device_7_MAC{""},device_8_MAC{""}{}

UserEntity::UserEntity(const std::string &username, int page_num)
        : username{username}, page_num{page_num} {}

std::string UserEntity::getUsername() const { return username; }
std::string UserEntity::getHashedPassword() const { return hashed_password; }
uint32_t UserEntity::getSalt() const { return salt; }

int UserEntity::getpage_num() const { return page_num;};




void UserEntity::set_device_1_MAC(const std::string &mac){device_1_MAC = mac;};
std::string UserEntity::get_device_1_MAC() const { return device_1_MAC;};

void UserEntity::set_device_2_MAC(const std::string &mac) { device_2_MAC = mac; };
std::string UserEntity::get_device_2_MAC() const { return device_2_MAC;};

void UserEntity::set_device_3_MAC(const std::string &mac) { device_3_MAC = mac; };
std::string UserEntity::get_device_3_MAC() const { return device_3_MAC;};

void UserEntity::set_device_4_MAC(const std::string &mac) { device_4_MAC = mac; };
std::string UserEntity::get_device_4_MAC() const { return device_4_MAC;};

void UserEntity::set_device_5_MAC(const std::string &mac) { device_5_MAC = mac; };
std::string UserEntity::get_device_5_MAC() const { return device_5_MAC;};

void UserEntity::set_device_6_MAC(const std::string &mac) { device_6_MAC = mac; };
std::string UserEntity::get_device_6_MAC() const { return device_6_MAC;};

void UserEntity::set_device_7_MAC(const std::string &mac) { device_7_MAC = mac; };
std::string UserEntity::get_device_7_MAC() const { return device_7_MAC;};

void UserEntity::set_device_8_MAC(const std::string &mac) { device_8_MAC = mac; };
std::string UserEntity::get_device_8_MAC() const { return device_8_MAC;};

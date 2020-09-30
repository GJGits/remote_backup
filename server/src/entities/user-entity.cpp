#include "../../include/entities/user-entity.hpp"

UserEntity::UserEntity(const std::string &username,
                       const std::string &hashed_password, uint32_t salt,
                       const std::string &device1,
                       const std::string &device2,
                       const std::string &device3,
                       int numdevices)
        : username{username}, hashed_password{hashed_password}, salt{salt}, device1{device1}, device2{device2}, device3{device3}, numdevices{numdevices} {}

UserEntity::UserEntity(const std::string &username,
                       const std::string &hashed_password, uint32_t salt )
    : username{username}, hashed_password{hashed_password}, salt{salt} {}

UserEntity::UserEntity(const std::string &username,
                       const std::string &hashed_password, uint32_t salt,
                       const std::string &device1)
        : username{username}, hashed_password{hashed_password}, salt{salt}, device1{device1} {}

UserEntity::UserEntity(const std::string &username, int page_num)
        : username{username}, page_num{page_num} {}

std::string UserEntity::getUsername() const { return username; }
std::string UserEntity::getHashedPassword() const { return hashed_password; }
uint32_t UserEntity::getSalt() const { return salt; }
std::string UserEntity::getDevice1() const { return device1; }
std::string UserEntity::getDevice2() const { return device2; }
std::string UserEntity::getDevice3() const { return device3; }
int UserEntity::getnumdevices() const { return numdevices; }
int UserEntity::getpage_num() const { return page_num;};

void UserEntity::setDevices(int actual_num_fields, std::string mac) {
    numdevices = actual_num_fields;
    if(actual_num_fields == 2)
        device2 = mac;
    else
        device3 = mac;
}

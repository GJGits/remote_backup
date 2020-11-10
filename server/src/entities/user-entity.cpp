#include "../../include/entities/user-entity.hpp"


UserEntity::UserEntity(const std::string &username,
                       const std::string &hashed_password, uint32_t salt)
        : username{username}, hashed_password{hashed_password}, salt{salt},
          device_1_MAC{""},device_2_MAC{""},device_3_MAC{""},
          device_4_MAC{""},device_5_MAC{""},device_6_MAC{""},
          device_7_MAC{""},device_8_MAC{""}{}



Subject UserEntity::get_subject() const { return subject; }
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

void UserEntity::set_db_selected(const size_t db) { db_selected = db; };
size_t UserEntity::get_db_selected() const { return db_selected;};

int UserEntity::check_validity_id(const std::string &mac) {
    if (get_device_1_MAC().compare(mac) == 0)
        return 1;
    else if (get_device_1_MAC().empty()) {
        set_device_1_MAC(mac);
        return -1;
    } else if (get_device_2_MAC().compare(mac) == 0) {
        return 2;
    } else if (get_device_2_MAC().empty()) {
        set_device_2_MAC(mac);
        return -2;
    } else if (get_device_3_MAC().compare(mac) == 0) {
        return 4;
    } else if (get_device_3_MAC().empty()) {
        set_device_3_MAC(mac);
        return -4;
    } else if (get_device_4_MAC().compare(mac) == 0) {
        return 8;
    } else if (get_device_4_MAC().empty()) {
        set_device_4_MAC(mac);
        return -8;
    } else if (get_device_5_MAC().compare(mac) == 0) {
        return 16;
    } else if (get_device_5_MAC().empty()) {
        set_device_5_MAC(mac);
        return -16;
    } else if (get_device_6_MAC().compare(mac) == 0) {
        return 32;
    } else if (get_device_6_MAC().empty()) {
        set_device_6_MAC(mac);
        return -32;
    } else if (get_device_7_MAC().compare(mac) == 0) {
        return 64;
    } else if (get_device_7_MAC().empty()) {
        set_device_7_MAC(mac);
        return -64;
    } else if (get_device_8_MAC().compare(mac) == 0) {
        return 128;
    } else if (get_device_8_MAC().empty()) {
        set_device_8_MAC(mac);
        return -128;
    }
    throw ExceededNumberOfDevices();
}
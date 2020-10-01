#pragma once
#include "../entities/db-connect.hpp"
#include "../exceptions/exceptions.hpp"

class DBRepository {
private:
    static inline std::shared_ptr<DBRepository> instance{nullptr};

    std::unordered_map<std::string, int> users_db_map;


public:
    static std::shared_ptr<DBRepository> getInstance();

    int getDBbyUsername(const std::string &username);

    size_t insertUsernameInDB(const std::string &username);
};
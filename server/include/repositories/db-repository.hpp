#pragma once
#include "../entities/db-connect.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/singleton.hpp"


class DBRepository : public Singleton<DBRepository>{
private:
    friend class Singleton;

public:

    int getDBbyUsername(const std::string &username);

    size_t insertUsernameInDB(const std::string &username);
};
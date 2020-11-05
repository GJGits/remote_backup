#pragma once
#include <memory>
#include <optional>
#include <cmath>
#include "../common/json.hpp"
#include "../common/constants.hpp"
#include "../common/sha256.hpp"
#include "../common/utility.hpp"
#include "../entities/db-connect.hpp"
#include "../entities/user-entity.hpp"
#include "../exceptions/exceptions.hpp"
#include "common-repository-function.hpp"
#include "../common/singleton.hpp"
#include "db-repository.hpp"


using json = nlohmann::json;

class UserRepository : public Singleton<UserRepository>{
private:
    friend class Singleton;

public:
  size_t insertUser(const UserEntity &user);
  UserEntity getUserByUsername(const std::string &username);
  bool UserAlreadyPresent(const std::string &username);
  json get_status_file(const Subject &subject, int page_num, int last_check);
  void updateUser(const UserEntity &user);
};
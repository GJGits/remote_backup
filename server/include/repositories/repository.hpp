#pragma once

#include <list>
#include <string>

#include "../entities/db-connect.hpp"

class Repository {
private:
  std::unique_ptr<sql::Statement> prepare_query(/*args*/) { return nullptr; }

protected:
  bool update_query(std::string &query,
                    std::list<std::string> &entries_of_query,
                    size_t db_selected) {
    // todo: completare
    return true;
  }

  std::unique_ptr<sql::ResultSet>
  select_query(std::string &query, std::list<std::string> &entries_of_query,
               size_t db_selected) {
    // todo: completare
    return nullptr;
  }
};
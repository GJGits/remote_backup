#pragma once
#include "../entities/db-connect.hpp"
#include "../common/utility.hpp"

class CommonRepository {

public:

    static std::unique_ptr<sql::Statement> common_preparation_query(std::string &query,
                                                             std::list<std::string> &entries_of_query,
                                                             size_t db_selected){

        std::unique_ptr<sql::Statement> stmt;
        std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn =
                DBConnect::getConnection(db_selected);
        int i, size = entries_of_query.size();
        for (i = 0; i < size; i++) {
            entries_of_query.push_back(
                    mysqlConn->escapeString(entries_of_query.front()));
            entries_of_query.pop_front();
        }
        std::vector<std::string> query_splitted = Utility::split(query, '?');
        i = 0;
        query = {};
        size = query_splitted.size();
        while (size > 1) {
            query = query + query_splitted[i] + entries_of_query.front();
            i++;
            entries_of_query.pop_front();
            size--;
        }
        query = query + query_splitted[i];
        return std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())};

    }


  static bool update_query(std::string &query,
                           std::list<std::string> &entries_of_query,
                           size_t db_selected) {

    common_preparation_query(query, entries_of_query, db_selected)->executeQuery(query);
    return true;
  }

  static std::unique_ptr<sql::ResultSet>
  select_query(std::string &query, std::list<std::string> &entries_of_query,
               size_t db_selected) {

    return nullptr;
  }
};
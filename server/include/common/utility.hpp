#pragma once
#include <string>
#include <vector>
#include <list>
#include <memory>
#include "../entities/db-connect.hpp"

class Utility {

public:
  static std::vector<std::string> split(const std::string &src,
                                        const char del) {
    std::string temp;
    std::vector<std::string> tokens;
    if (!src.empty()) {
      for (const char &c : src) {
        if (c != del)
          temp += c;
        else {
          tokens.push_back(temp);
          temp = "";
        }
      }
      if (!temp.empty())
        tokens.push_back(temp);
    }

    return tokens;
  }

  static std::string split_string(std::string &s, std::string &delimiter){
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(1, pos);
            s.erase(0, pos + delimiter.length());
        }
        return s;
    }



  static std::string directories_path(const std::string &src,
                                 const char del) {
        std::string temp;
        std::vector<std::string> tokens;
        std::string dirs_path;
        if (!src.empty()) {
            for (const char &c : src) {
                if (c != del)
                    temp += c;
                else {
                    tokens.push_back(temp);
                    temp = "";
                }
            }
            if (!temp.empty())
                tokens.push_back(temp);
            for(size_t i = 0 ; i < tokens.size()-1 ; i++){
                dirs_path = dirs_path + tokens[i];
                dirs_path = dirs_path + "/";
            }
        }
        return dirs_path;
    }

    static bool update_query(std::string &query,std::list<std::string> &entries_of_query, size_t db_selected){
        std::unique_ptr<sql::Statement> stmt;
        std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(db_selected);
        int i,size = entries_of_query.size();

        for(i=0 ; i<size ; i++){
            entries_of_query.push_back(mysqlConn->escapeString(entries_of_query.front()));
            entries_of_query.pop_front();
        }
        std::vector<std::string> query_splitted = Utility::split(query,'?');
        i=0;
        query={};
        size = query_splitted.size();
        while(size>1){
            query = query + query_splitted[i] + entries_of_query.front();
            i++;
            entries_of_query.pop_front();
            size--;
        }
        query = query + query_splitted[i];
        stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore
        stmt->execute(query);

        return true;
    }

    static std::unique_ptr<sql::ResultSet> select_query(std::string &query,std::list<std::string> &entries_of_query, size_t db_selected){
        std::unique_ptr<sql::Statement> stmt;
        std::shared_ptr<sql::mysql::MySQL_Connection> mysqlConn = DBConnect::getConnection(db_selected);
        int i,size = entries_of_query.size();

        for(i=0 ; i<size ; i++){
            entries_of_query.push_back(mysqlConn->escapeString(entries_of_query.front()));
            entries_of_query.pop_front();
        }
        std::vector<std::string> query_splitted = Utility::split(query,'?');
        i=0;
        query={};
        size = query_splitted.size();
        while(size>1){
            query = query + query_splitted[i] + entries_of_query.front();
            i++;
            entries_of_query.pop_front();
            size--;
        }
        query = query + query_splitted[i];
        stmt = std::unique_ptr<sql::Statement>{std::move(mysqlConn->createStatement())}; // ricordare al posto di 0, di mettere il vero valore

        return std::unique_ptr<sql::ResultSet>{std::move(stmt->executeQuery(query))};
    }

};

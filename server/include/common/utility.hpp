#pragma once
#include <string>
#include <vector>

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

};

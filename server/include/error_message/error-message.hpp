#include <iostream>
#include <fstream>
#pragma once

enum error_enum {
    ok,
    user_already_exist,
    wrong_credentials,
    username_not_exist,
    generic_error,
    pass_passconfirm_neq
};

class Error_message{

private:
    std::map<error_enum, std::string> error_map;

    inline static Error_message *instance = nullptr;

    Error_message(std::string filename){

        std::ifstream error_file;
        error_file.open (filename);
        std::string line;
        int i=0;
        while (getline(error_file,line)) {
            switch (i){
                case 0 :
                    error_map[ok] = line;
                    break;
                case 1 :
                    error_map[user_already_exist] = line;
                    break;
                case 2 :
                    error_map[wrong_credentials] = line;
                    break;
                case 3 :
                    error_map[username_not_exist] = line;
                    break;
                case 4 :
                    error_map[generic_error] = line;
                    break;
                case 5 :
                    error_map[pass_passconfirm_neq] = line;
                    break;
                default:
                    break;

            }
            i++;
        }

    }
public:
    static Error_message *getInstance(std::string filename = "../include/error_message/error_message.txt") {
        if (instance == nullptr) {
            instance = new Error_message(filename);
        }
        return instance;
    }


    std::string get_error_message(error_enum en){
        return error_map[en];
    };


};

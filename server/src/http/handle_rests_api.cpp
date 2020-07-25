//
// Created by ontes on 25/07/20.
//
#include <string>
#include <mutex>
#include "../../include/http/handle_rests_api.hpp"
rests_api_get_status* rests_api_get_status::instance = nullptr;

std::string rests_api_get_status::get_and_produce(){
    std::unique_lock lk(m);
    while(1){
        if(!requests.empty()){
            std::string new_req = requests.front();
            requests.pop();
            std::clog << "ok, c'è una roba da prendere e buttar fuori\n";
            /* Produzione di qualcosa con tale request */
            std::string new_rep = new_req + "helloooo\n";
            replies.push(new_rep);
            /* Per ora la ritorno pure, oltre a metterla in un'altra coda, poi si vede cosa faremo */
            return new_rep;
        }
        else{
            cv.wait(lk,[this](){
                if(requests.empty())
                    return false;
                else
                    return true;
            });
        }
    }
}

void rests_api_get_status::add_and_wakeup(std::string req){
    std::unique_lock lk(m);
    requests.push(req);
    cv.notify_one();
}



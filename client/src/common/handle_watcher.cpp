//
// Created by alessandra on 30/07/20.
//

#include "../../include/common/handle_watcher.hpp"



void Handle_watcher::handle_InCreate(std::string event){
     std::clog<<" Evento: InCreate , cartella : " << event << "\n";

}

void Handle_watcher::handle_InDelete(std::string event){
    std::clog<<" Evento: InDelete, cartella :" << event << "\n";
}

void Handle_watcher::handle_InModify(std::string event){
    std::clog<<" Evento: InModify, cartella :" << event << "\n";
}

void Handle_watcher::handle_InRename(std::string event){
    std::clog<<" Evento: InRename, cartella :" << event << "\n";
}
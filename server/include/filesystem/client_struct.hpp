#pragma once
#include "../common/json.hpp"
#include "../common/sha256.hpp"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>



using json = nlohmann::json;

class ClientStruct {

private:
    std::string file_clientstruct_path;
    std::string username;
    bool file_found=false;
    bool chunk_found=false;
    int index=0;
    std::unique_ptr<json> structure;




public:

    void read_structure() {
        if (structure.get() == nullptr || (*structure).empty()) {
            std::clog << "e0\n";
            structure = std::make_unique<json>();
            std::clog << "e1\n";
            std::ifstream i(file_clientstruct_path);
            i >> (*structure);
            std::clog << "e2\n";
        }
    }

     ClientStruct(std::string username): username(username){
         file_clientstruct_path = "../../filesystem/" + username + "/client-struct.json";
         std::clog << "il fileclientstructpath è : " << file_clientstruct_path << "\n";
         read_structure();
     };




    void research_file(std::string path){
        std::clog << "e3\n";
        if (!(*structure)["entries"].empty()) {
            std::clog << "e4\n";
            std::clog << "la dimensione e': " << (*structure)["entries"].size() << "\n";
            for (size_t i = 0; i < (*structure)["entries"].size(); i++) {
                std::clog << "il dump è: " << (*structure)["entries"][0].dump() << "\n";
                json tmp = (*structure)["entries"][i];
                std::clog << "provo su " << tmp["path"] << "\n";
                if (tmp["path"].get<std::string>().compare(path) ==
                    0) { // C'è già il path nel client-struct.json
                    file_found = true;
                    break;
                    std::clog << "file trovato\n";
                }
                index++;
            }
        }
    }


    void add_chunk(std::vector<char> chunk_body,std::string file_path, std::string chunk_id, std::string chunk_hash, std::string chunk_size){
        std::clog << "il file c'è già e dobbiamo solo aggiungere i chunks \n";

        json chunk;
        chunk["id"] = chunk_id;
        chunk["hash"] = chunk_hash; //todo: ok calcolare l'hash dal blocco di dati, ma confrontarlo con l'hash ricevuto
        /*bool found_chunk = false;
        for (size_t i = 0; i < (*structure)["entries"][index]["chunks"].size();
             i++) {
            if ((*structure)["entries"][index]["chunks"][i]["id"] == chunk["id"]) {
                found_chunk = true;
                (*structure)["entries"][index]["chunks"][i] = chunk;
            }
        }
        if (!found_chunk) {
         */
            (*structure)["entries"][index]["chunks"].push_back(chunk);

        //}

        (*structure)["entries"][index]["size"] = -1; //todo: mettere il file size senzato, quando capirò come creare effettivamente i files dal chunk
        (*structure)["entries"][index]["validity"] = false;
        (*structure)["entries"][index]["dim_last_chunk"] = chunk_size;

    }

    void modify_chunk(std::vector<char> chunk_body, std::string chunk_id, std::string chunk_hash, std::string chunk_size){
        std::clog << "il file c'è già e dobbiamo solo aggiungere i chunks \n";

        json chunk;
        chunk["id"] = chunk_id;
        chunk["hash"] = chunk_hash; //todo: ok calcolare l'hash dal blocco di dati, ma confrontarlo con l'hash ricevuto
        bool found_chunk = false;
        for (size_t i = 0; i < (*structure)["entries"][index]["chunks"].size();
             i++) {
            if ((*structure)["entries"][index]["chunks"][i]["id"] == chunk["id"]) {
                found_chunk = true;
                (*structure)["entries"][index]["chunks"][i] = chunk;
            }
        }
        if (!found_chunk) {
            (*structure)["entries"][index]["chunks"].push_back(chunk);
        }

        (*structure)["entries"][index]["size"] = -1; //todo: mettere il file size senzato, quando capirò come creare effettivamente i files dal chunk
        (*structure)["entries"][index]["validity"] = false;
        (*structure)["entries"][index]["dim_last_chunk"] = chunk_size;

    }


    void created_new_file(std::string username, std::string chunk_hash, std::string chunk_id, std::string path, std::string file_size, bool validity, std::string chunk_size, std::string timestamp_locale, std::vector<char> chunk_body){
        json entry;
        json chunk;
        std::clog << "e5\n";
        int full_chunk_size = 50;
        chunk["id"] = chunk_id;
        chunk["hash"] = chunk_hash;

        entry["chunks"].push_back(chunk);
        entry["path"] = path;
        entry["size"] = file_size; //todo: mettere il file size senzato, quando capirò come creare effettivamente i files dal chunk
        entry["validity"] = validity;
        entry["dim_last_chunk"] = chunk_size;
        entry["last_mod"] = timestamp_locale;
        std::clog << "e6\n";

        std::ofstream outfile;
        outfile.open("../../filesystem/"+username+"/"+path);

        std::vector<char> padding(full_chunk_size, '0'); //todo: recuperare la vera chunk size
        for(int i = 0 ; i <= std::stoi(chunk_id) ; i++){
            if(i == std::stoi(chunk_id))
                for(auto b : chunk_body)
                outfile << b;
            else{
                outfile << padding.data();
            }
        }
        outfile.close();


        (*structure)["entries"].push_back(entry);
    }

    void update_total_file_status(){
        std::string entries_dump = (*structure)["entries"].dump();
        std::vector<char> data(entries_dump.begin(), entries_dump.end());
        (*structure)["hashed_status"] = (*structure)["entries"].empty()
                                        ? std::string{"empty_hashed_status"}
                                        : Sha256::getSha256(data);
    }

    void write_structure(){
        std::ofstream o(file_clientstruct_path);
        o << (*structure) << "\n";
        o.close();
       // (*structure).clear(); essendo in stack, uscendo dalla funzione si distrugge tutto, però rivedere sta cosa
    }

    bool get_file_found(){ return file_found;}
};
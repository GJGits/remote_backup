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
    std::vector<int> chunks_already_present;
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
                    std::clog << "file trovato\n";
                    break;
                }
                index++;
            }
        }
    }

    bool delete_file(std::string file_path,std::string username ){
        std::clog << "e3\n";
        if (!(*structure)["entries"].empty()) {
            std::clog << "e4\n";
            std::clog << "la dimensione e': " << (*structure)["entries"].size() << "\n";
            for (size_t i = 0; i < (*structure)["entries"].size(); i++) {
                std::clog << "il dump è: " << (*structure)["entries"][0].dump() << "\n";
                json tmp = (*structure)["entries"][i];
                std::clog << "provo su " << tmp["path"] << "\n";
                if (tmp["path"].get<std::string>().compare(file_path) ==
                    0) {
                    (*structure)["entries"].erase(i);
                    remove(("../../filesystem/"+username+"/"+file_path).c_str());
                    std::clog << "file trovato\n";
                    return true;
                }
            }
        }
        return false;
    }



    void add_chunk(std::vector<char> chunk_body,std::string file_path, std::string chunk_id, std::string chunk_hash, std::string chunk_size){
        std::clog << "il file c'è già e dobbiamo solo aggiungere i chunks \n";
        int full_chunk_size = 10;
/*
        json chunk;
        chunk["id"] = chunk_id;
        chunk["hash"] = chunk_hash;

        (*structure)["entries"][index]["chunks"].push_back(chunk);
*/

        for (size_t i = 0; i < (*structure)["entries"][index]["chunks"].size();
             i++) {
            std::string ind = (*structure)["entries"][index]["chunks"][i]["id"];
            chunks_already_present.push_back(std::stoi(ind));
        }

        std::ofstream outfile;
        outfile.open("../../filesystem/"+username+"/"+file_path , std::ios_base::in | std::ios_base::out | std::ios_base::ate);

        std::string padding(full_chunk_size, '0');
        int i = 0;
        for(i = 0 ; i < std::stoi(chunk_id) ; i++){
            if(!std::count(chunks_already_present.begin(),chunks_already_present.end(),i)){ //cerco se quel chunk_id è già stato scritto, se non lo è, devo 0 fillare
                outfile.seekp(i*full_chunk_size);
                outfile.write(padding.c_str(),padding.size());
            }
        }
        outfile.seekp(i*full_chunk_size);
        std::string strvec{chunk_body.begin(),chunk_body.end()};
        outfile.write(strvec.c_str(),strvec.size());

        outfile.close();

        (*structure)["entries"][index]["size"] = "-1"; //todo: calcolare sta size
        (*structure)["entries"][index]["validity"] = false;
        (*structure)["entries"][index]["dim_last_chunk"] = chunk_size;

    }


    void modify_chunk(std::vector<char> chunk_body, std::string chunk_id, std::string chunk_hash, std::string chunk_size, std::string file_path){
        std::clog << "il file c'è già e dobbiamo solo aggiungere i chunks \n";
        //int full_chunk_size = 2097152;
        int full_chunk_size = 10;

        json chunk;
        chunk["id"] = chunk_id;
        chunk["hash"] = chunk_hash; //todo: ok calcolare l'hash dal blocco di dati, ma confrontarlo con l'hash ricevuto

        std::ofstream outfile;
        outfile.open("../../filesystem/"+username+"/"+file_path , std::ios_base::in | std::ios_base::out | std::ios_base::ate);


        for (size_t i = 0; i < (*structure)["entries"][index]["chunks"].size();
             i++) {
            if ((*structure)["entries"][index]["chunks"][i]["id"] == chunk["id"]) {
                (*structure)["entries"][index]["chunks"][i] = chunk;
                break;
            }
        }

        outfile.seekp(std::stoi(chunk_id)*full_chunk_size);
        std::string strvec{chunk_body.begin(),chunk_body.end()};
        outfile.write(strvec.c_str(),strvec.size());
        outfile.close();

        (*structure)["entries"][index]["size"] = "-1"; //todo: calcolare sta size
        (*structure)["entries"][index]["validity"] = false;
        (*structure)["entries"][index]["dim_last_chunk"] = chunk_size;

    }


    void created_new_file(std::string username, std::string chunk_hash, std::string chunk_id, std::string file_path, bool validity, std::string chunk_size, std::string timestamp_locale, std::vector<char> chunk_body){
        json entry;
        json chunk;

        int full_chunk_size = 10;
        chunk["id"] = chunk_id;
        chunk["hash"] = chunk_hash;

        entry["chunks"].push_back(chunk);
        entry["path"] = file_path;
        entry["validity"] = validity;
        entry["dim_last_chunk"] = chunk_size;
        entry["last_mod"] = timestamp_locale;

        std::ofstream outfile;
        outfile.open("../../filesystem/"+username+"/"+file_path);

        std::string padding(full_chunk_size, '0');
        int i=0;
        for(i = 0 ; i < std::stoi(chunk_id) ; i++){
            if(i != std::stoi(chunk_id)){
                outfile.seekp(i*full_chunk_size, std::ios_base::beg);
                outfile << padding;
            }
        }
        outfile.seekp(i*full_chunk_size, std::ios_base::beg);
        std::string strvec{chunk_body.begin(),chunk_body.end()};
        outfile << strvec;
        outfile.close();



        entry["size"] = (i)*full_chunk_size + std::stoi(chunk_size);

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

    std::string get_chunk(std::string chunk_id, std::string file_path,std::string chunk_size, std::string username){

        std::fstream file(("../../filesystem/"+username+"/"+file_path), std::ios::in );
        char F[std::stoi(chunk_size)+1];
        file.seekg(std::stoi(chunk_id)*std::stoi(chunk_size), std::ios::beg);
        file.read(F, std::stoi(chunk_size));
        F[std::stoi(chunk_size)] = 0;
        std::string s(F);
        file.close();
        return s;

    }


    bool delete_chunk(std::string file_path,std::string username, std::string chunk_id ) {
        if (std::stoi(chunk_id) == 0){
            remove(("../../filesystem/" + username + "/" + file_path).c_str());
            return true;
        }
        int full_chunk_size = 10;
        std::ofstream outfile;
        std::string path{"../../filesystem/"+username+"/"+file_path};
        outfile.open(path , std::ios_base::in | std::ios_base::out | std::ios_base::ate);
        int dim=std::stoi(chunk_id)*full_chunk_size;
        std::filesystem::resize_file(path,dim);
        outfile.close();
        return true;
    }


    bool get_file_found(){ return file_found;}
};
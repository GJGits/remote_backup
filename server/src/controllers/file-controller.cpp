#include "../../include/controllers/file-controller.hpp"

inline static std::regex post_file_rgx{"^\\/file\\/.*\\/.*\\/.*$"};

const http::server::reply
FileController::handle(const http::server::request &req) {

    if (req.method == "POST") { //todo: migliorare la selezione dello username, invece di prendere tutti i campi all'inizio

        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, post_file_rgx)) { //todo: La regex prende un pò tutto, migliorare

            PostFileDTO post_file{};
            post_file.fill(req.uri,req.body);

        std::string s{req.uri};
        std::vector<std::string> uri_elements = Utility::split(s, '/');
        uri_elements[5] = macaron::Base64::Decode(uri_elements[5]);

        if(1) { //todo: verificare che il client sia autenticato

            std::string path{"../../filesystem/" + uri_elements[2] + "/client-struct.json"};
            if (std::filesystem::exists(path)) {
                bool file_found = false;

                std::unique_ptr<json> structure;
                int index = 0;
                structure = std::make_unique<json>();
                std::ifstream i(path);
                i >> (*structure);
                if (!(*structure)["entries"].empty()) {
                    for (size_t i = 0; i < (*structure)["entries"].size(); i++) {
                        index++;
                        json tmp = (*structure)["entries"][i];
                        if (tmp["path"].get<std::string>().compare(uri_elements[5]) ==
                            0) { // C'è già il path nel client-struct.json
                            file_found = true;
                            break;
                        }
                    }
                }
                if (file_found) {
                    std::clog << "il file c'è già e dobbiamo solo aggiungere i chunks e fare le modifiche\n";

                } else { // File appena creato, nuovo, non ci stava prima
                    json entry;
                    json chunk;
                    std::string hash_chunk = Sha256::getSha256(req.body);
                    chunk["id"] = uri_elements[3];
                    chunk["hash"] = hash_chunk;
                    entry["chunks"].push_back(chunk);
                    entry["path"] = uri_elements[5];
                    entry["size"] = -1;
                    entry["validity"] = false;
                    entry["dim_last_chunk"] = uri_elements[4];
                    entry["last_mod"] = uri_elements[6];

                    (*structure)["entries"].push_back(entry);


                    if ((*structure)["entries"].empty()) {
                        (*structure)["hashed_status"] = std::string{"empty_hashed_status"};
                    } else {
                        std::string entries_dump = (*structure)["entries"].dump();
                        std::vector<char> data(entries_dump.begin(), entries_dump.end());
                        (*structure)["hashed_status"] = (*structure)["entries"].empty()
                                                        ? std::string{"empty_hashed_status"}
                                                        : Sha256::getSha256(data);
                    }

                    std::ofstream o(path);
                    o << (*structure) << "\n";
                    o.close();
                    (*structure).clear();
                }


            }
            return MakeReply::make_1line_jsonReply<std::string>("token", "tutto bene", http::server::reply::ok);

        }

        }
/*
        if (req.uri == "/auth/signin") {

            std::string req_body = std::string{req.body.begin(), req.body.end()};
            std::clog << "req_body = " << req_body << "\n";
            std::smatch match;
            if (!std::regex_match(req_body, match, signin_rgx))
                throw WrongRquestFormat(); // todo: creare eccezione

            SigninDTO user{};
            user.deserialize(req_body);
            return MakeReply::make_1line_jsonReply<std::string>("token", post_signin(user), http::server::reply::ok);


        } else if (req.uri == "/auth/signup") {

            std::string req_body = std::string{req.body.begin(), req.body.end()};
            std::smatch match;
            if (!std::regex_match(req_body, match, signup_rgx))
                throw WrongRquestFormat(); // todo: creare eccezione

            SignupDTO user{}; // specifica
            user.deserialize(req_body);
            return MakeReply::make_1line_jsonReply<std::string>("token", post_signup(user), http::server::reply::ok);
        }*/
    std::clog << "Sono in file-controller POST\n";
    }
    if (req.method == "PUT"){
        std::clog << "Sono in file-controller PUT\n";
    }
    if (req.method == "GET"){
        std::clog << "Sono in file-controller GET\n";
    }
    return MakeReply::make_1line_jsonReply<std::string>("token", "tutto ok", http::server::reply::ok);


    throw WrongRquestFormat(); // todo: creare eccezione

};

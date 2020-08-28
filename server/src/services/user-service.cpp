#include "../../include/services/user-service.hpp"

std::string UserService::login(const SigninDTO &user) {
  UserRepository user_rep;
  UserEntity user_returned =
      user_rep.getUserByUsername(user.getUsername());

    unsigned int salt = user_returned.getSalt();
    std::string password_to_hash{std::to_string(salt) + user.getPassword() + std::to_string(salt)};
    std::vector<char> vec(password_to_hash.begin(), password_to_hash.end());
    std::string hashed_password{Sha256::getSha256(vec)};
    if (user_returned.getHashedPassword().compare(hashed_password) == 0)
      return JWT::generateToken(user.getUsername(), JWT::getExpiration() + std::time(nullptr));
    else
      throw CredentialsNotValidException();


  throw UknownError();
}

std::string UserService::signup(const SignupDTO &user) {

  /* Eventualmente tale check se deve essere fatto più avanti lo si sposta*/
  if (user.getPassword().compare(user.getPasswordConfirm()) != 0)
    throw PasswordNeqConfirm();

  UserRepository user_rep;
  std::string username(user.getUsername());
  unsigned int salt = abs(rand());
    std::string password_to_hash{std::to_string(salt) + user.getPassword() + std::to_string(salt)};
    std::vector<char> vec(password_to_hash.begin(), password_to_hash.end());
    std::string hashedPassword{Sha256::getSha256(vec)};
  UserEntity user_to_insert{username, hashedPassword, salt};
  if (user_rep.insertUser(user_to_insert)) {
    /* Create user directory */
    std::string path{"../../filesystem/" + username};
    std::filesystem::create_directories(path);

    /* Create client-struct.json file */
    json j ;
    j["hashed_status"] = "empty_hashed_status";
    j["entries"] = json::array();
    std::ofstream o(path + "/client-struct.json");
    o << std::setw(4) << j << std::endl;
    return JWT::generateToken(user.getUsername(), JWT::getExpiration() + std::time(nullptr));
  } else
    throw UsernameAlreadyExists();
}

std::string UserService::getStatus(const std::string &username) {
  UserRepository user_rep;
  user_rep.getUserByUsername(username);
    json j;
    std::ifstream i("../../filesystem/" + username + "/client-struct.json");
    i >> j;
    return j["hashed_status"];

}

std::string UserService::getStatusFile(const std::string &username) {
    UserRepository user_rep;
    user_rep.getUserByUsername(username);
    json j;
    std::ifstream i("../../filesystem/" + username + "/client-struct.json");
    i >> j;
    return j.dump();

}

std::string UserService::file_chunk_add(const PostFileDTO &post_file) {

    ClientStruct clientstr(post_file.getusername());
    clientstr.research_file(post_file.getfile_path());

    if (clientstr.get_file_found())
        clientstr.add_or_modify_chunk(post_file.getchunk_id(),Sha256::getSha256(post_file.getchunk_body()),post_file.getchunk_size());
    else
        clientstr.created_new_file(Sha256::getSha256(post_file.getchunk_body()), post_file.getchunk_id(),post_file.getfile_path(), "-1", false, post_file.getchunk_size(), post_file.gettimestamp_locale());

    clientstr.update_total_file_status();
    clientstr.write_structure();

    return "ciao";

}



std::string UserService::file_chunk_update(const PutFileDTO &put_file) {

    if(1) { //todo: verificare che il client sia autenticato
    std::clog << "Sono in file_chunk_update\n";

        std::string path{"../../filesystem/" + put_file.getusername() + "/client-struct.json"};
        if (std::filesystem::exists(path)) { //todo: In teoria esiste sempre, capirne la necessità
            std::unique_ptr<json> structure;
            int index = 0;
            structure = std::make_unique<json>();
            std::ifstream i(path);
            i >> (*structure);
            if (!(*structure)["entries"].empty()) {
                for (size_t i = 0; i < (*structure)["entries"].size(); i++) {
                    json tmp = (*structure)["entries"][i];
                    if (tmp["path"].get<std::string>().compare(put_file.getfile_path()) ==
                        0) { // C'è già il path nel client-struct.json
                        std::clog << "file trovato\n";
                        break;
                    }
                    index++;
                }
            }

                json chunk;
                chunk["id"] = put_file.getchunk_id();
                chunk["hash"] = Sha256::getSha256(put_file.getchunk_body()); //todo: ok calcolare l'hash dal blocco di dati, ma confrontarlo con l'hash ricevuto
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
            (*structure)["entries"][index]["dim_last_chunk"] = put_file.getchunk_size();


            // Aggiorno il file_status
            std::string entries_dump = (*structure)["entries"].dump();
            std::vector<char> data(entries_dump.begin(), entries_dump.end());
            (*structure)["hashed_status"] = (*structure)["entries"].empty()
                                            ? std::string{"empty_hashed_status"}
                                            : Sha256::getSha256(data);

            std::ofstream o(path);
            o << (*structure) << "\n";
            o.close();
            (*structure).clear();

        }
        return "è la vita\n";

    }

    return "ciao";

}

std::string UserService::file_chunk_get(const GetFileDTO &get_file) {

    if(1) { //todo: verificare che il client sia autenticato


        std::string path{"../../filesystem/" + get_file.getusername() + "/client-struct.json"};
        if (std::filesystem::exists(path)) { //todo: In teoria esiste sempre, capirne la necessità
            bool file_found = false;

            std::unique_ptr<json> structure;
            int index = 0;
            structure = std::make_unique<json>();
            std::ifstream i(path);
            i >> (*structure);
            std::clog << "Il file da trovare è: " << get_file.getfile_path() << "\n";
            if (!(*structure)["entries"].empty()) {
                for (size_t i = 0; i < (*structure)["entries"].size(); i++) {
                    json tmp = (*structure)["entries"][i];
                    std::clog << "provo su " << tmp["path"] << "\n";
                    if (tmp["path"].get<std::string>().compare(get_file.getfile_path()) ==
                        0) { // C'è già il path nel client-struct.json
                        file_found = true;
                        break;
                        std::clog << "file trovato\n";
                    }
                    index++;
                }
            }
            if (file_found) {
                std::clog << "il file c'è e dobbiamo tornare i bytes relativi\n";

                //todo: calcolare e ritornare il chunk corrispondente

            } else { // File appena creato, nuovo, non ci stava prima

                //todo: ritornare qualcosa di vuoto in termini di blocco dati

            }

        }
        return "è la vita\n";

    }

    return "ciao";


}

#include "../../include/services/user-service.hpp"

int div_ceil(int numerator, int denominator)
{
    std::div_t res = std::div(numerator, denominator);
    return res.rem ? (res.quot + 1) : res.quot;
}

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
    std::clog <<"il dump è : " << j.dump() << "\n";
    return j.dump();

}

std::string UserService::file_chunk_add(const PostChunkDTO &post_file) {
    std::clog << "l'hash vero è: "<< Sha256::getSha256(post_file.getchunk_body()) << "\n";

    int full_chunk_size = 10;

    if(Sha256::getSha256(post_file.getchunk_body()).compare(post_file.getchunk_hash()) == 0){
        int filesize = stoi(post_file.getnumber_of_chunks()) - full_chunk_size + std::stoi(post_file.getchunk_size());
        ChunkEntity chunk_ent{post_file.getusername(), std::stoi(post_file.getchunk_id()), post_file.getchunk_hash(), post_file.getfile_path(), std::stoi(post_file.getchunk_size()), post_file.gettimestamp_locale(), filesize};
        ChunkRepository chunk_rep;

        std::ofstream outfile;

        std::vector<char> chunk_body{post_file.getchunk_body()};
        std::string padding(full_chunk_size, '0');
        int i=0;
        if(chunk_rep.getFilePath(chunk_ent)==false) {

            outfile.open("../../filesystem/"+post_file.getusername()+"/"+post_file.getfile_path());
            std::clog << "Non Trovato\n";
            for(i = 0 ; i < std::stoi(post_file.getchunk_id()) ; i++){
                if(i != std::stoi(post_file.getchunk_id())){
                    outfile.seekp(i*full_chunk_size, std::ios_base::beg);
                    outfile << padding;
                }
            }

            chunk_rep.addFileInfo(chunk_ent);
        }
        else {
            outfile.open("../../filesystem/"+post_file.getusername()+"/"+post_file.getfile_path() , std::ios_base::in | std::ios_base::out | std::ios_base::ate);

            //Inserire la size, calcolarla e metterla nella chunk_ent
            int max_id = div_ceil(chunk_rep.getFileSize(chunk_ent),full_chunk_size);
            for(i = max_id ; i < std::stoi(post_file.getchunk_id()); i++){
                    outfile.seekp(i*full_chunk_size);
                    outfile.write(padding.c_str(),padding.size());
            }
            std::clog << "Trovato\n";
           // chunk_rep.updateFileInfo(chunk_ent); //todo: DA FARE

        }

        outfile.seekp(i*full_chunk_size, std::ios_base::beg);
        std::string strvec{chunk_body.begin(),chunk_body.end()};
        outfile << strvec;
        outfile.close();
        // Devo riaggiornare le info del file ogni volta
        chunk_rep.addChunk(chunk_ent);

        return "200_OK";

    }

    return "chunk_corrupted";

}



std::string UserService::file_chunk_update(const PutChunkDTO &put_file) {
    std::clog << "l'hash vero è: "<< Sha256::getSha256(put_file.getchunk_body()) << "\n";

    if(Sha256::getSha256(put_file.getchunk_body()).compare(put_file.getchunk_hash()) == 0) {

        ClientStruct clientstr(put_file.getusername());
        clientstr.research_file(put_file.getfile_path());
        clientstr.modify_chunk(put_file.getchunk_body(),put_file.getchunk_id(), put_file.getchunk_hash(),
                                      put_file.getchunk_size(),put_file.getfile_path());
        clientstr.update_total_file_status();
        clientstr.write_structure();

        return "200_OK";
    }
    else
        return "chunk_corrupted";

}

std::string UserService::file_chunk_get(const GetChunkDTO &get_file) {

    ClientStruct clientstr(get_file.getusername());
    clientstr.research_file(get_file.getfile_path());

    return clientstr.get_chunk(get_file.getchunk_id(),get_file.getfile_path(),get_file.getchunk_size(),get_file.getusername());

}


std::string UserService::delete_file_service(const DeleteFileDTO &del_file) {

    ClientStruct clientstr(del_file.getusername());
    if(clientstr.delete_file(del_file.getfile_path(), del_file.getusername()) == false){
        return "File not eliminated\n";
    }
    clientstr.update_total_file_status();
    clientstr.write_structure();
    return "200_OK";
}


std::string UserService::file_chunk_delete_service(const DeleteChunkDTO &del_chunk){

    ClientStruct clientstr(del_chunk.getusername());

    if(clientstr.delete_chunk(del_chunk.getfile_path(), del_chunk.getusername(), del_chunk.getchunk_id()) == false){
        return "File not eliminated\n";
    }

    return "200_OK";
}


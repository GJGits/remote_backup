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
  return user_rep.update_hashed_status(username);

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
        ChunkEntity chunk_ent{post_file.getusername(), post_file.getchunk_id(), post_file.getchunk_hash(), post_file.getfile_path(), post_file.getchunk_size(), post_file.gettimestamp_locale()};
        ChunkRepository chunk_rep;

        std::ofstream outfile;

        std::vector<char> chunk_body{post_file.getchunk_body()};
        std::string padding(full_chunk_size, '0');
        int i=0;
        int filesize = 0;
        if(chunk_rep.getFilePath(chunk_ent)==false) {
            filesize = (post_file.getchunk_id()-1)*full_chunk_size + post_file.getchunk_size();
            chunk_ent.setSizeFile(filesize);
            outfile.open("../../filesystem/"+post_file.getusername()+"/"+post_file.getfile_path());
            for(i = 0 ; i < post_file.getchunk_id() ; i++){
                if(i != post_file.getchunk_id()){
                    outfile.seekp(i*full_chunk_size, std::ios_base::beg);
                    outfile << padding;
                }
            }

            chunk_rep.addFileInfo(chunk_ent);

        }
        else {
            outfile.open("../../filesystem/"+post_file.getusername()+"/"+post_file.getfile_path() , std::ios_base::in | std::ios_base::out | std::ios_base::ate);

            int max_id = div_ceil(chunk_rep.getFileSize(chunk_ent),full_chunk_size);
            if(post_file.getchunk_id() >= max_id){
                for(i = max_id ; i < post_file.getchunk_id(); i++){
                        outfile.seekp(i*full_chunk_size);
                        outfile.write(padding.c_str(),padding.size());
                }

                filesize = (post_file.getchunk_id()-1)*full_chunk_size + post_file.getchunk_size();
                chunk_ent.setSizeFile(filesize);

            }
            else {

                filesize = chunk_rep.getFileSize(chunk_ent);
                chunk_ent.setSizeFile(filesize);
                i = post_file.getchunk_id();
            }
            chunk_rep.updateFileInfo(chunk_ent);
        }


        outfile.seekp(i*full_chunk_size, std::ios_base::beg);
        std::string strvec{chunk_body.begin(),chunk_body.end()};
        outfile.write(strvec.c_str(),strvec.size());
        outfile.close();
        chunk_rep.addChunk(chunk_ent);

        return "200_OK";

    }

    return "chunk_corrupted";

}



std::string UserService::file_chunk_update(const PutChunkDTO &put_file) {
    std::clog << "l'hash vero è: "<< Sha256::getSha256(put_file.getchunk_body()) << "\n";

    if(Sha256::getSha256(put_file.getchunk_body()).compare(put_file.getchunk_hash()) == 0) {
        int full_chunk_size = 10;

        ChunkEntity chunk_ent{put_file.getusername(), put_file.getchunk_id(), put_file.getchunk_hash(), put_file.getfile_path(), put_file.getchunk_size(),put_file.gettimestamp_locale()};
        ChunkRepository chunk_rep;

        std::ofstream outfile;
        outfile.open("../../filesystem/"+put_file.getusername()+"/"+put_file.getfile_path() , std::ios_base::in | std::ios_base::out | std::ios_base::ate);
        int max_id = div_ceil(chunk_rep.getFileSize(chunk_ent),full_chunk_size);

        std::vector<char> chunk_body{put_file.getchunk_body()};
        outfile.seekp(put_file.getchunk_id()*full_chunk_size, std::ios_base::beg);
        std::string strvec{chunk_body.begin(),chunk_body.end()};
        outfile.write(strvec.c_str(),strvec.size());
        chunk_rep.updateChunk(chunk_ent);
        int filesize = 0;
        if(max_id == put_file.getchunk_id()) {
            int dim = (max_id - 1) * full_chunk_size + put_file.getchunk_size();
            std::filesystem::resize_file("../../filesystem/"+put_file.getusername()+"/"+put_file.getfile_path(), dim);

            filesize = (put_file.getchunk_id()-1)*full_chunk_size + put_file.getchunk_size();
        }
        else{
            filesize = chunk_rep.getFileSize(chunk_ent);
        }
        chunk_ent.setSizeFile(filesize);
        chunk_rep.updateFileInfo(chunk_ent);

        outfile.close();

        return "200_OK";
    }
    else
        return "chunk_corrupted";

}

std::string UserService::file_chunk_get(const GetChunkDTO &get_file) {
    int full_chunk_size = 10;
    std::fstream file(("../../filesystem/"+get_file.getusername()+"/"+get_file.getfile_path()), std::ios::in );
    char F[get_file.getchunk_size()+1];
    file.seekg(get_file.getchunk_id()*full_chunk_size, std::ios::beg);
    file.read(F, get_file.getchunk_size());
    F[get_file.getchunk_size()] = 0;
    std::string s(F);
    file.close();
    return s;
}


std::string UserService::delete_file_service(const DeleteFileDTO &del_file) {

    FileEntity file_ent{del_file.getusername(),del_file.getfile_path()};
    FileRepository file_rep;
    if(file_rep.deleteFile(file_ent)){
        remove(("../../filesystem/" + del_file.getusername() + "/" + del_file.getfile_path()).c_str());
        return "200_OK";
    }


    return "File_not_eliminated";
}


std::string UserService::file_chunk_delete_service(const DeleteChunkDTO &del_chunk){

    ChunkEntity chunk_ent{del_chunk.getusername(), del_chunk.getchunk_id(), del_chunk.getfile_path(), del_chunk.getchunk_size(),del_chunk.gettimestamp_locale()};
    ChunkRepository chunk_rep;

    chunk_rep.delete_chunks(chunk_ent);
    int full_chunk_size = 10;
    int filesize = (del_chunk.getchunk_id()-1) * full_chunk_size;
    chunk_ent.setSizeFile(filesize);
    chunk_rep.updateFileInfo(chunk_ent);
    std::filesystem::resize_file("../../filesystem/"+del_chunk.getusername()+"/"+del_chunk.getfile_path(), filesize);
    return "200_OK";
}


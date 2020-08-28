#include "../../include/controllers/file-controller.hpp"

inline static std::regex post_file_rgx{"^\\/file\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)$"};
inline static std::regex put_get_file_rgx{"^\\/file\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)$"};

const http::server::reply
FileController::handle(const http::server::request &req) {

    if (req.method == "POST") { //todo: migliorare la selezione dello username, invece di prendere tutti i campi all'inizio

        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, post_file_rgx)) { //todo: La regex prende un pò tutto, migliorare

           // if (JWT::validateToken(req)) {
                PostFileDTO post_file{};
                post_file.fill(req.uri, req.body);
                post_file_chunk(post_file);
           /* }
            else
                throw CredentialsExpired();*/
        }
        else
            throw WrongRquestFormat();

    }
    if (req.method == "PUT"){
        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, put_get_file_rgx)) { //todo: La regex prende un pò tutto, migliorare

           // if (JWT::validateToken(req)) {
            PutFileDTO put_file{};
            put_file.fill(req.uri,req.body);
            put_file_chunk(put_file);
            /*}
            else
                throw CredentialsExpired();*/
        }
        else
            throw WrongRquestFormat();
    }
    if (req.method == "GET"){
        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, put_get_file_rgx)) { //todo: La regex prende un pò tutto, migliorare

            //if (JWT::validateToken(req)) {
            GetFileDTO get_file{};
            get_file.fill(req.uri);
            get_file_chunk(get_file);
            /*}
            else
                throw CredentialsExpired();*/
        }
        else
            throw WrongRquestFormat();
    }
    return MakeReply::make_1line_jsonReply<std::string>("token", "tutto ok", http::server::reply::ok);


    throw WrongRquestFormat(); // todo: creare eccezione

};

 std::string FileController::post_file_chunk(const PostFileDTO &post_file) {

    UserService *user_service = UserService::getInstance();
    return user_service->file_chunk_add(post_file);
}

std::string FileController::put_file_chunk(const PutFileDTO &put_file) {

    UserService *user_service = UserService::getInstance();
    return user_service->file_chunk_update(put_file);
}

std::string FileController::get_file_chunk(const GetFileDTO &get_file) {

    UserService *user_service = UserService::getInstance();
    return user_service->file_chunk_get(get_file);
}

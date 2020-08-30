#include "../../include/controllers/chunk-controller.hpp"

inline static std::regex post_chunk_rgx{"^\\/chunk\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)$"};
inline static std::regex put_get_chunk_rgx{"^\\/chunk\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)$"};

const http::server::reply
ChunkController::handle(const http::server::request &req) {

    if (req.method == "POST") { //todo: migliorare la selezione dello username, invece di prendere tutti i campi all'inizio

        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, post_chunk_rgx)) { //todo: La regex prende un pò tutto, migliorare

           // if (JWT::validateToken(req)) {
                PostChunkDTO post_chunk{};
                post_chunk.fill(req.uri, req.body);
                std::string response = post_file_chunk(post_chunk);
                if(response.compare("200_OK")==0)
                    return http::server::reply::stock_reply(http::server::reply::ok);
                else
                    return MakeReply::make_1line_jsonReply<std::string>("err_msg", response, http::server::reply::bad_request);

           /* }
            else
                throw CredentialsExpired();*/

        }
        else
            throw WrongRquestFormat();

    }
    if (req.method == "PUT"){
        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, put_get_chunk_rgx)) { //todo: La regex prende un pò tutto, migliorare

           // if (JWT::validateToken(req)) {
            PutChunkDTO put_chunk{};
            put_chunk.fill(req.uri,req.body);
            std::string response = put_file_chunk(put_chunk);
            if(response.compare("200_OK")==0)
                return http::server::reply::stock_reply(http::server::reply::ok);
            else
                return MakeReply::make_1line_jsonReply<std::string>("err_msg", response, http::server::reply::bad_request);

            /*}
            else
                throw CredentialsExpired();*/
        }
        else
            throw WrongRquestFormat();
    }
    if (req.method == "GET"){
        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, put_get_chunk_rgx)) { //todo: La regex prende un pò tutto, migliorare

            //if (JWT::validateToken(req)) {
            GetChunkDTO get_chunk{};
            get_chunk.fill(req.uri);
            get_file_chunk(get_chunk);
            /*}
            else
                throw CredentialsExpired();*/
        }
        else
            throw WrongRquestFormat();
    }
    return MakeReply::make_1line_jsonReply<std::string>("token", "tutto ok", http::server::reply::bad_request);


    throw WrongRquestFormat(); // todo: creare eccezione

};

 std::string ChunkController::post_file_chunk(const PostChunkDTO &post_chunk) {

    UserService *user_service = UserService::getInstance();
    return user_service->file_chunk_add(post_chunk);
}

std::string ChunkController::put_file_chunk(const PutChunkDTO &put_chunk) {

    UserService *user_service = UserService::getInstance();
    return user_service->file_chunk_update(put_chunk);
}

std::string ChunkController::get_file_chunk(const GetChunkDTO &get_chunk) {

    UserService *user_service = UserService::getInstance();
    return user_service->file_chunk_get(get_chunk);
}

#include "../../include/controllers/chunk-controller.hpp"

inline static std::regex post_chunk_rgx{
    "^\\/chunk\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+\\S+)\\/"
    "(\\w+)$"};
inline static std::regex put_chunk_rgx{
    "^\\/chunk\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+\\S+)\\/(\\w+)$"};
inline static std::regex get_chunk_rgx{
    "^\\/chunk\\/(\\w+)\\/(\\w+)\\/(\\w+)\\/(\\w+\\S+)$"};
inline static std::regex delete_chunk_rgx{"^\\/chunk\\/(\\w+)\\/(\\w+\\S+)$"};

const http::server::reply
ChunkController::handle(const http::server::request &req) {

  if (req.method == "POST") { // todo: migliorare la selezione dello username,
                              // invece di prendere tutti i campi all'inizio

    std::smatch match;
    if (std::regex_search(
            req.uri.begin(), req.uri.end(), match,
            post_chunk_rgx)) { // todo: La regex prende un pò tutto, migliorare

      // if (JWT::validateToken(req)) {
      PostChunkDTO post_chunk{};
      post_chunk.fill(req.uri, req.body);
      std::string response = post_file_chunk(post_chunk);
      if (response.compare("200_OK") == 0)
        return http::server::reply::stock_reply(http::server::reply::ok);
      else
        return MakeReply::make_1line_jsonReply<std::string>(
            "err_msg", response, http::server::reply::bad_request);

      /* }
       else
           throw CredentialsExpired();*/

    } else
      throw WrongRquestFormat();
  }
  if (req.method == "PUT") {
    std::smatch match;
    if (std::regex_search(
            req.uri.begin(), req.uri.end(), match,
            put_chunk_rgx)) { // todo: La regex prende un pò tutto, migliorare

      // if (JWT::validateToken(req)) {
      PutChunkDTO put_chunk{};
      put_chunk.fill(req.uri, req.body);
      std::string response = put_file_chunk(put_chunk);
      if (response.compare("200_OK") == 0)
        return http::server::reply::stock_reply(http::server::reply::ok);
      else
        return MakeReply::make_1line_jsonReply<std::string>(
            "err_msg", response, http::server::reply::bad_request);

      /*}
      else
          throw CredentialsExpired();*/
    } else
      throw WrongRquestFormat();
  }
  if (req.method == "GET") {
    std::smatch match;
    if (std::regex_search(
            req.uri.begin(), req.uri.end(), match,
            get_chunk_rgx)) { // todo: La regex prende un pò tutto, migliorare

      // if (JWT::validateToken(req)) {
      GetChunkDTO get_chunk{};
      get_chunk.fill(req.uri);
      return MakeReply::make_1line_dump_jsonReply<std::string>(
          get_file_chunk(
              get_chunk), // todo: al momento ritorna una roba strana,
                          // aggiustare il return tornando solo il json del file
          http::server::reply::ok);
      /*}
      else
          throw CredentialsExpired();*/
    } else
      throw WrongRquestFormat();
  }

  if (req.method == "DELETE") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match,
                          delete_chunk_rgx)) { // todo: La regex prende un pò
                                               // tutto, migliorare

      // if (JWT::validateToken(req)) {
      DeleteChunkDTO delete_chunk{};
      delete_chunk.fill(req.uri);
      std::string response = delete_file_chunk(delete_chunk);
      if (response.compare("200_OK") == 0)
        return http::server::reply::stock_reply(http::server::reply::ok);
      else
        return MakeReply::make_1line_jsonReply<std::string>(
            "err_msg", response, http::server::reply::bad_request);

      /*}
      else
          throw CredentialsExpired();*/
    } else
      throw WrongRquestFormat();
  }

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

std::string
ChunkController::delete_file_chunk(const DeleteChunkDTO &delete_chunk) {

  UserService *user_service = UserService::getInstance();
  return user_service->file_chunk_delete_service(delete_chunk);
}

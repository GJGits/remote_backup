#include "../../include/controllers/chunk-controller.hpp"

inline static std::regex post_chunk_rgx{
    "^\\/chunk\\/[\\w]+\\/[\\w]+\\/[\\w]+\\/[\\w=+]+\\/[\\w]+$"};
inline static std::regex put_chunk_rgx{
    "^\\/chunk\\/[\\w]+\\/[\\w]+\\/[\\w]+\\/[\\w=+]+\\/[\\w]+$"};
inline static std::regex get_chunk_rgx{
    "^\\/chunk\\/[\\w]+\\/[\\w]+\\/[\\w]+\\/[\\w=+]+$"};
inline static std::regex delete_chunk_rgx{
    "^\\/chunk\\/[\\w]+\\/[\\w=+]+$"};

std::shared_ptr<ChunkController> ChunkController::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<ChunkController>(new ChunkController{});
    instance->chunk_service = ChunkService::getInstance();
  }
  return instance;
}

const http::server::reply
ChunkController::handle(const http::server::request &req) {

  Subject sub = JWT::validateToken(req);

  if (req.method == "POST") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match,
                          post_chunk_rgx)) {
      PostChunkDTO post_chunk{sub};
      post_chunk.fill(req);
      post_file_chunk(post_chunk);
      return http::server::reply::stock_reply(http::server::reply::ok);
    }
  } else if (req.method == "PUT") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match,
                          put_chunk_rgx)) {
      PutChunkDTO put_chunk{sub};
      put_chunk.fill(req);
      put_file_chunk(put_chunk);
      return http::server::reply::stock_reply(http::server::reply::ok);
    }
  } else if (req.method == "GET") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match,
                          get_chunk_rgx)) {

      GetChunkDTO get_chunk{};
      get_chunk.fill(req.uri);
      return MakeReply::make_1line_dump_jsonReply<std::string>(
          get_file_chunk(get_chunk), http::server::reply::ok);
    }
  }

  else if (req.method == "DELETE") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match,
                          delete_chunk_rgx)) {

      DeleteChunkDTO delete_chunk{sub};
      delete_chunk.fill(req.uri);
      delete_file_chunk(delete_chunk);
      return http::server::reply::stock_reply(http::server::reply::ok);
    }
  }
  throw WrongRquestFormat();
};

void ChunkController::post_file_chunk(const PostChunkDTO &post_chunk) {
  chunk_service->file_chunk_add(post_chunk);
}

void ChunkController::put_file_chunk(const PutChunkDTO &put_chunk) {
  chunk_service->file_chunk_update(put_chunk);
}

std::string ChunkController::get_file_chunk(const GetChunkDTO &get_chunk) {
  return chunk_service->file_chunk_get(get_chunk);
}

void ChunkController::delete_file_chunk(const DeleteChunkDTO &delete_chunk) {
  chunk_service->file_chunk_delete_service(delete_chunk);
}

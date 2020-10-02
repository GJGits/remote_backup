#include "../../include/controllers/chunk-controller.hpp"

inline static std::regex post_chunk_rgx{
    "^\\/chunk\\/[\\w]+\\/[\\w]+\\/[\\w]+\\/[\\w]+\\/[\\w=+]+\\/[\\w]+$"};
inline static std::regex put_chunk_rgx{
    "^\\/chunk\\/[\\w]+\\/[\\w]+\\/[\\w]+\\/[\\w=+]+\\/[\\w]+$"};
inline static std::regex get_chunk_rgx{
    "^\\/chunk\\/[\\w]+\\/[\\w=+]+$"};
inline static std::regex delete_chunk_rgx{"^\\/chunk\\/[\\w]+\\/[\\w=+]+$"};

std::shared_ptr<ChunkController> ChunkController::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<ChunkController>(new ChunkController{});
    instance->chunk_service = ChunkService::getInstance();
  }
  return instance;
}

const http::server::reply
ChunkController::handle(const http::server::request &req) {
std::clog << "provo la handle\n";
  Subject sub = JWT::validateToken(req);
    std::clog << "try la handle\n";

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

      GetChunkDTO get_chunk{sub};
      get_chunk.fill(req.uri);
      http::server::reply rep =
          http::server::reply::stock_reply(http::server::reply::ok);
      get_chunk.link_content_buffer(rep.content);
      size_t size = get_file_chunk(get_chunk);
      struct http::server::header con_len;
      con_len.name = "Content-Length";
      con_len.value = std::to_string(size);
      struct http::server::header con_type;
      con_type.name = "Content-Type";
      con_type.value = "application/stream";
      rep.headers.push_back(con_len);
      rep.headers.push_back(con_type);
      return rep;
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

size_t ChunkController::get_file_chunk(const GetChunkDTO &get_chunk) {
  return chunk_service->file_chunk_get(get_chunk);
}



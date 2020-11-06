#include "../../include/controllers/chunk-controller.hpp"

const std::regex ChunkController::get_post_chunk_rgx(){return post_chunk_rgx;}
const std::regex ChunkController::get_get_chunk_rgx(){return get_chunk_rgx;}

const http::server::reply
ChunkController::handle(const http::server::request &req) {
  Subject sub = JWT::validateToken(req);

  if (req.method == "POST") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match,
                          get_post_chunk_rgx())) {
      PostChunkDTO post_chunk{sub};
      post_chunk.fill(req, (*req.body).size());
      post_file_chunk(post_chunk);
      return http::server::reply::stock_reply_empty(http::server::reply::ok);
    }
  } else if (req.method == "GET") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match,
                          get_get_chunk_rgx())) {

      GetChunkDTO get_chunk{sub};
      get_chunk.fill(req.uri);
      http::server::reply rep = http::server::reply::stock_reply(http::server::reply::ok);
      get_chunk.link_content_buffer(rep.content);
      MakeReply::makebinaryreply(rep, get_file_chunk(get_chunk));
      return rep;
    }
  }
  throw WrongRquestFormat();
};

void ChunkController::post_file_chunk(const PostChunkDTO &post_chunk) {
  DurationLogger logger{"POST_CHUNK"};
  std::shared_ptr<ChunkService> chunk_service = ChunkService::getInstance();
  chunk_service->file_chunk_add(post_chunk);
}

size_t ChunkController::get_file_chunk(const GetChunkDTO &get_chunk) {
  std::shared_ptr<ChunkService> chunk_service = ChunkService::getInstance();
  return chunk_service->file_chunk_get(get_chunk);
}

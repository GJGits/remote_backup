#pragma once
#include "controller.hpp"

class ChunkController : public Controller, public Singleton<ChunkController> {

private:
  friend class Singleton;
  std::regex post_chunk_rgx;
  std::regex get_chunk_rgx;
  ChunkController()
      : post_chunk_rgx{std::move(std::regex{
            "^\\/chunk\\/[\\w]+\\/[\\w]+\\/[\\w]+\\/[\\w=+]+\\/[\\w]+$"})},
        get_chunk_rgx{std::move(std::regex{"^\\/chunk\\/[\\w]+\\/[\\w=+]+$"})}
            {}

public:
  virtual const http::server::reply handle(const http::server::request &req);
  void post_file_chunk(const PostChunkDTO &post_chunk);
  size_t get_file_chunk(const GetChunkDTO &get_chunk);
  const std::regex get_post_chunk_rgx();
  const std::regex get_get_chunk_rgx();

};
#pragma once
#include "../common/makereply.hpp"
#include "../common/singleton.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../services/chunk-service.hpp"
#include "../common/jwt.hpp"
#include "controller.hpp"
#include <regex>

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
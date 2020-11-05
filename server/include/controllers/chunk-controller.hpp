#pragma once
#include "../common/base64.hpp"
#include "../common/makereply.hpp"
#include "../common/sha256.hpp"
#include "../common/singleton.hpp"
#include "../common/utility.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/signup_dto.hpp"
#include "../services/chunk-service.hpp"
#include "../services/user-service.hpp"
#include "controller.hpp"
#include <regex>

class ChunkController : public Controller, public Singleton<ChunkController> {

private:
  friend class Singleton;
  std::regex post_chunk_rgx;
  ChunkController()
      : post_chunk_rgx{std::move(std::regex{
            "^\\/chunk\\/[\\w]+\\/[\\w]+\\/[\\w]+\\/[\\w=+]+\\/[\\w]+$"})} {}

public:
  virtual const http::server::reply handle(const http::server::request &req);
  void post_file_chunk(const PostChunkDTO &post_chunk);
  size_t get_file_chunk(const GetChunkDTO &get_chunk);
};
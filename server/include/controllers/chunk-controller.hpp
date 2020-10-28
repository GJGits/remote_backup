#pragma once
#include "controller.hpp"
#include "../common/makereply.hpp"
#include "../common/base64.hpp"
#include "../common/sha256.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../services/user-service.hpp"
#include "../services/chunk-service.hpp"
#include <regex>
#include "../common/utility.hpp"
#include "../common/singleton.hpp"

class ChunkController : public Controller, public Singleton<ChunkController> {

private:
    friend class Singleton;
    std::shared_ptr<ChunkService> chunk_service;
    ChunkController(){    this->chunk_service = ChunkService::getInstance();}

public:
    virtual const http::server::reply handle(const http::server::request &req);
    void post_file_chunk(const PostChunkDTO &post_chunk);
    size_t get_file_chunk(const GetChunkDTO &get_chunk);
};
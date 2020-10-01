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

class ChunkController : public Controller {

private:
    static inline std::shared_ptr<ChunkController> instance{nullptr};
    std::shared_ptr<ChunkService> chunk_service;
public:
    static std::shared_ptr<ChunkController> getInstance();
    virtual const http::server::reply handle(const http::server::request &req);
    void post_file_chunk(const PostChunkDTO &post_chunk);
    void put_file_chunk(const PutChunkDTO &put_chunk);
    void get_file_chunk(const GetChunkDTO &get_chunk);

};
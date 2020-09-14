#pragma once
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/put_chunk_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/delete_chunk_dto.hpp"
#include "../common/sha256.hpp"
#include "../repositories/chunk-repository.hpp"
#include "../entities/chunk-entity.hpp"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "../exceptions/exceptions.hpp"
#include "../common/jwt.hpp"
#include "../common/utility.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ctime>
#include "../common/constants.hpp"


class ChunkService {
private:
    static inline std::shared_ptr<ChunkService> instance{nullptr};
public:
    std::mutex mtx;
    static std::shared_ptr<ChunkService> getInstance();
    void file_chunk_add(const PostChunkDTO &post_chunk);
    void file_chunk_update(const PutChunkDTO &put_chunk);
    std::string file_chunk_get(const GetChunkDTO &get_file);
    void file_chunk_delete_service(const DeleteChunkDTO &del_chunk);
    ~ChunkService() {}
};

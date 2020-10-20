#include "../../include/dtos/put_chunk_dto.hpp"

Subject PutChunkDTO::get_subject() const {return subject;}

std::string PutChunkDTO::getold_path_64() const { return old_path_64; }
std::string PutChunkDTO::getold_file_path_64() const { return old_file_path_64; }
std::string PutChunkDTO::getnew_path_64() const { return new_path_64; }
std::string PutChunkDTO::getnew_file_path_64() const { return new_file_path_64; }


const void PutChunkDTO::fill(const http::server::request &req) {
    std::vector<std::string> uri_elements = Utility::split(req.uri, '/');
    old_path_64 = std::move(uri_elements[2]);
    old_file_path_64 = "../../filesystem/" + get_subject().get_sub() + "/" + old_path_64;
    new_path_64 = std::move(uri_elements[3]);
    new_file_path_64 = "../../filesystem/" + get_subject().get_sub() + "/" + new_path_64;
}
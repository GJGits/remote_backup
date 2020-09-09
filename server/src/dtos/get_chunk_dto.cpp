#include "../../include/dtos/get_chunk_dto.hpp"

std::string GetChunkDTO::getusername() const { return username; }
std::string GetChunkDTO::getchunk_id() const { return chunk_id; }
std::string GetChunkDTO::getchunk_size() const { return chunk_size; }
std::string GetChunkDTO::getfile_path() const { return file_path; }


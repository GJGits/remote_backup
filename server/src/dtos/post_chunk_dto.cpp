#include "../../include/dtos/post_chunk_dto.hpp"

std::string PostChunkDTO::getusername() const { return username; }
std::string PostChunkDTO::getchunk_id() const { return chunk_id; }
std::string PostChunkDTO::getchunk_size() const { return chunk_size; }
std::string PostChunkDTO::getchunk_hash() const { return chunk_hash; }
std::string PostChunkDTO::getfile_path() const { return file_path; }
std::string PostChunkDTO::gettimestamp_locale() const { return timestamp_locale; }
std::vector<char> PostChunkDTO::getchunk_body() const { return chunk_body; }


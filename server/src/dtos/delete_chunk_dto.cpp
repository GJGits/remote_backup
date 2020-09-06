#include "../../include/dtos/delete_chunk_dto.hpp"

std::string DeleteChunkDTO::getusername() const { return username; }
int DeleteChunkDTO::getchunk_id() const { return chunk_id; }
int DeleteChunkDTO::getchunk_size() const { return chunk_size; }
std::string DeleteChunkDTO::getfile_path() const { return file_path; }
std::string DeleteChunkDTO::gettimestamp_locale() const { return timestamp_locale; }


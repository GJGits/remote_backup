#include "../../include/dtos/put_chunk_dto.hpp"

std::string PutChunkDTO::getusername() const { return username; }
int PutChunkDTO::getchunk_id() const { return chunk_id; }
int PutChunkDTO::getchunk_size() const { return chunk_size; }
std::string PutChunkDTO::getchunk_hash() const { return chunk_hash; }
std::string PutChunkDTO::getfile_path() const { return file_path; }
std::string PutChunkDTO::getnumber_of_chunks() const { return number_of_chunks; }
std::vector<char> PutChunkDTO::getchunk_body() const { return chunk_body; }
std::string PutChunkDTO::gettimestamp_locale() const { return timestamp_locale; }



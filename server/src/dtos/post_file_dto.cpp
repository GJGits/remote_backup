#include "../../include/dtos/post_file_dto.hpp"

std::string PostFileDTO::getusername() const { return username; }
std::string PostFileDTO::getchunk_id() const { return chunk_id; }
std::string PostFileDTO::getchunk_size() const { return chunk_size; }
std::string PostFileDTO::getfile_path() const { return file_path; }
std::string PostFileDTO::gettimestamp_locale() const { return timestamp_locale; }
std::vector<char> PostFileDTO::getchunk_body() const { return chunk_body; }


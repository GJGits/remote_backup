#include "../../include/dtos/put_file_dto.hpp"

std::string PutFileDTO::getusername() const { return username; }
std::string PutFileDTO::getchunk_id() const { return chunk_id; }
std::string PutFileDTO::getchunk_size() const { return chunk_size; }
std::string PutFileDTO::getfile_path() const { return file_path; }
std::vector<char> PutFileDTO::getchunk_body() const { return chunk_body; }


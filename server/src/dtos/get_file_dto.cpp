#include "../../include/dtos/get_file_dto.hpp"

std::string GetFileDTO::getusername() const { return username; }
std::string GetFileDTO::getchunk_id() const { return chunk_id; }
std::string GetFileDTO::getchunk_size() const { return chunk_size; }
std::string GetFileDTO::getfile_path() const { return file_path; }


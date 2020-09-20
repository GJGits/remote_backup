#include "../../include/dtos/delete_file_dto.hpp"

Subject DeleteFileDTO::get_subject() const { return subject; }
std::string DeleteFileDTO::getfile_path() const { return file_path; }

const void DeleteFileDTO::fill(const http::server::request &req) {
  std::vector<std::string> uri_elements = Utility::split(req.uri, '/');
  file_path = "../../filesystem/" + get_subject().get_sub() + "/" +
              std::move(uri_elements[2]);
}
#include "../../include/controllers/file-controller.hpp"

inline static std::regex delete_rgx{"^\\/file\\/[\\w=+]+$"};


const http::server::reply
FileController::handle(const http::server::request &req) {
  Subject sub = JWT::validateToken(req);
  if (req.method == "DELETE") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match, delete_rgx)) {
      DeleteFileDTO del_file{sub};
      del_file.fill(req);
      delete_file(del_file);
        return http::server::reply::stock_reply_empty(http::server::reply::ok);

    }
  }
  throw WrongRquestFormat();
};

void FileController::delete_file(const DeleteFileDTO &del_file) {
  file_service->delete_file_service(del_file);
}

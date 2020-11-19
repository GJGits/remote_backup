#include "../../include/controllers/file-controller.hpp"

const std::regex FileController::get_delete_rgx(){return delete_rgx;};

const http::server::reply
FileController::handle(const http::server::request &req) {
  std::shared_ptr<JWT> jwt = JWT::getInstance();
  Subject sub = jwt->validateToken(req);
  if (req.method == "DELETE") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match, get_delete_rgx())) {
      DeleteFileDTO del_file{sub};
      del_file.fill(req);
      delete_file(del_file);
        return http::server::reply::stock_reply_empty(http::server::reply::ok);

    }
  }
  throw WrongRquestFormat();
};

void FileController::delete_file(const DeleteFileDTO &del_file) {

    std::shared_ptr<FileService> file_service = FileService::getInstance();
    file_service->delete_file_service(del_file);

}

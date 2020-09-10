#include "../../include/controllers/file-controller.hpp"

inline static std::regex delete_rgx{"^\\/file\\/(\\w+)\\/(\\w+\\S+)$"};

const http::server::reply
FileController::handle(const http::server::request &req) {

    if (req.method == "DELETE") {
        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, delete_rgx)) {
            // if (JWT::validateToken(req)) {
            DeleteFileDTO del_file{};
            del_file.fill(req.uri, req.body);
            delete_file(del_file);
            return http::server::reply::stock_reply(http::server::reply::ok);
            /* }
             else
                 throw CredentialsExpired();*/
        }
    }
    throw WrongRquestFormat();

};


void FileController::delete_file(const DeleteFileDTO &del_file) {

  FileService *file_service = FileService::getInstance();
  file_service->delete_file_service(del_file);
  return;
}


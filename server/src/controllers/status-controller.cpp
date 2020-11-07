#include "../../include/controllers/status-controller.hpp"


const std::regex StatusController::get_user_rgx_file(){return user_rgx_file;}


const http::server::reply
StatusController::handle(const http::server::request &req) {
  Subject sub = JWT::validateToken(req);
  if (req.method == "GET") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match,
                          user_rgx_file)) {
      GetStatusDTO get_status_dto{sub};
      get_status_dto.fill(req.uri);
      json list = get_status_file(get_status_dto);
      return MakeReply::make_1line_jsonReply<json>(
          "list", list, http::server::reply::ok);
    }
  }
  throw WrongRquestFormat();
}

// {"list": {}}

// {"error": {}}

const json
StatusController::get_status_file(const GetStatusDTO &get_status_dto) {
  std::shared_ptr<FileService> file_service = FileService::getInstance();
  return file_service->getStatusFile(get_status_dto);
}
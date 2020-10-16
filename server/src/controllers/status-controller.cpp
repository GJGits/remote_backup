#include "../../include/controllers/status-controller.hpp"

std::shared_ptr<StatusController> StatusController::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<StatusController>(new StatusController{});
    instance->file_service = FileService::getInstance();
  }
  return instance;
}

inline static std::regex user_rgx_file{"^\\/status\\/list\\/[\\d]+\\/[\\d]+$"};

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
        std::clog << "list dump server: " << list.dump() << "\n";
      return MakeReply::make_1line_dump_jsonReply<json>(
          list, http::server::reply::ok);
    }
  }
  throw WrongRquestFormat();
}


const json
StatusController::get_status_file(const GetStatusDTO &get_status_dto) {
  return file_service->getStatusFile(get_status_dto);
}
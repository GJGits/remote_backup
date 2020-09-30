#include "../../include/controllers/status-controller.hpp"

std::shared_ptr<StatusController> StatusController::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<StatusController>(new StatusController{});
    instance->user_service = UserService::getInstance();
    instance->file_service = FileService::getInstance();
  }
  return instance;
}

inline static std::regex user_rgx{"^/status$"};
inline static std::regex user_rgx_file{"^\\/status\\/[\\d]+\\/file$"};

const http::server::reply
StatusController::handle(const http::server::request &req) {
  Subject sub = JWT::validateToken(req);
  if (req.method == "GET") {
    std::smatch match;
    if (std::regex_search(req.uri.begin(), req.uri.end(), match, user_rgx)) {
      std::string username{std::move(match[1])};
      return MakeReply::make_1line_jsonReply<std::string>(
          std::string{"hashed_status"}, get_status(username),
          http::server::reply::ok);
    } else if (std::regex_search(req.uri.begin(), req.uri.end(), match,
                                 user_rgx_file)) {
        GetStatusDTO get_status_dto{sub};
        get_status_dto.fill(req.uri);
      return MakeReply::make_1line_dump_jsonReply<std::string>(
          get_status_file(get_status_dto), http::server::reply::ok);
    }
  }
  throw WrongRquestFormat();
}

const std::string StatusController::get_status(const std::string &username) {
  return user_service->getStatus(username);
}

const std::string
StatusController::get_status_file(const GetStatusDTO &get_status_dto) {

  return file_service->getStatusFile(get_status_dto);
}
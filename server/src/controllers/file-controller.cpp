#include "../../include/controllers/file-controller.hpp"

inline static std::regex delete_rgx{"^\\/file\\/(\\w+)\\/(\\w+\\S+)$"};



const http::server::reply
FileController::handle(const http::server::request &req) {

    if (req.method == "DELETE") { //todo: migliorare la selezione dello username, invece di prendere tutti i campi all'inizio

        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, delete_rgx)) { //todo: La regex prende un pò tutto, migliorare

            // if (JWT::validateToken(req)) {
            DeleteFileDTO del_file{};
            del_file.fill(req.uri, req.body);
            std::string response = delete_file(del_file);
            if(response.compare("200_OK")==0)
                return http::server::reply::stock_reply(http::server::reply::ok);
            else
                return MakeReply::make_1line_jsonReply<std::string>("err_msg", response, http::server::reply::bad_request);

            /* }
             else
                 throw CredentialsExpired();*/

        }
        else
            throw WrongRquestFormat();

    }


    throw WrongRquestFormat(); // todo: creare eccezione

};


const std::string FileController::delete_file(const DeleteFileDTO &del_file) {

  UserService *user_service = UserService::getInstance();
  return user_service->delete_file_service(del_file);
}


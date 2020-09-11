#pragma once
#include "controller.hpp"
#include "../common/makereply.hpp"
#include "../common/base64.hpp"
#include "../common/sha256.hpp"
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../services/user-service.hpp"
#include "../services/file-service.hpp"
#include <regex>
#include "../common/utility.hpp"

class FileController : public Controller {

private:
    static inline std::shared_ptr<FileController> instance{nullptr};

public:
    static std::shared_ptr<FileController> getInstance();

    virtual const http::server::reply handle(const http::server::request &req);
    void delete_file(const DeleteFileDTO &del_file);
};
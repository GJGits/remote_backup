#pragma once
#include "controller.hpp"

class FileController : public Controller, public Singleton<FileController> {

private:
    friend class Singleton;
    std::regex delete_rgx;

    FileController(): delete_rgx{std::move(std::regex{"^\\/file\\/[\\w=+]+\\/\\d+$"})}{}

public:

    virtual const http::server::reply handle(const http::server::request &req);
    void delete_file(const DeleteFileDTO &del_file);
    const std::regex get_delete_rgx();

};
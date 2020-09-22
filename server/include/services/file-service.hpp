#pragma once
#include "../dtos/signup_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/put_chunk_dto.hpp"
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/delete_file_dto.hpp"
#include "../dtos/delete_chunk_dto.hpp"

#include "../common/sha256.hpp"
#include "../filesystem/client_struct.hpp"
#include "../repositories/user-repository.hpp"
#include "../repositories/chunk-repository.hpp"
#include "../repositories/file-repository.hpp"
#include "../entities/chunk-entity.hpp"
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "../exceptions/exceptions.hpp"
#include "../common/jwt.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ctime>
#include "../common/constants.hpp"



class FileService {
private:
    static inline std::shared_ptr<FileService> instance{nullptr};
    std::shared_ptr<FileRepository> file_repository;
    std::shared_ptr<UserRepository> user_repository;
public:

    std::mutex mtx;
    static std::shared_ptr<FileService> getInstance();

    std::string getStatusFile(const std::string &username);
    void delete_file_service(const DeleteFileDTO &del_file);
    ~FileService() {}

};
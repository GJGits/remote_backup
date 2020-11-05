#pragma once
#include "../dtos/get_chunk_dto.hpp"
#include "../dtos/get_status_dto.hpp"
#include "../dtos/post_chunk_dto.hpp"
#include "../dtos/signin_dto.hpp"
#include "../dtos/signup_dto.hpp"

#include "../dtos/delete_file_dto.hpp"
#include "../common/singleton.hpp"
#include "../common/constants.hpp"
#include "../common/jwt.hpp"
#include "../common/sha256.hpp"
#include "../exceptions/exceptions.hpp"
#include "../repositories/chunk-repository.hpp"
#include "../repositories/file-repository.hpp"
#include "../repositories/user-repository.hpp"
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>

class FileService: public Singleton<FileService>  {
private:
    friend class Singleton;
    FileService(){}

public:
  std::mutex mtx;

  const json getStatusFile(const GetStatusDTO &get_status_dto);
  void delete_file_service(const DeleteFileDTO &del_file);
  ~FileService() {}
};
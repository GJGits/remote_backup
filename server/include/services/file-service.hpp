#pragma once
#include "../dtos/get_status_dto.hpp"
#include "../dtos/delete_file_dto.hpp"
#include "../common/singleton.hpp"
#include "../exceptions/exceptions.hpp"
#include "../repositories/file-repository.hpp"
#include "../repositories/user-repository.hpp"
#include "../common/json.hpp"
#include <filesystem>

using json = nlohmann::json;


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
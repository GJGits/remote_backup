#pragma once
#include "auth-controller.hpp"
#include "controller.hpp"
#include "status-controller.hpp"
#include "chunk-controller.hpp"
#include "file-controller.hpp"
#include "test-controller.hpp"

#include "../exceptions/exceptions.hpp"


class ControllerRouter {
private:
  inline static std::unordered_map<std::string, std::shared_ptr<Controller>> controller_map{};
    static inline std::shared_ptr<ControllerRouter> instance{nullptr};


public:


  static std::shared_ptr<Controller> getController(const std::string &key) {
    // se mappa non inizializzata
    if (instance.get() == nullptr) {
        instance = std::shared_ptr<ControllerRouter>(new ControllerRouter{});
      controller_map["auth"] = AuthController::getInstance();
      controller_map["status"] = StatusController::getInstance();
      controller_map["chunk"] = ChunkController::getInstance();
      controller_map["file"] = FileController::getInstance();
      controller_map["test"] = TestController::getInstance();


        // todo: inizializza
    }
    // qui map ok, recupero controller
    // todo: recupera controller qui:
    std::string analysis_stringa(key);
    char *res = std::strtok(const_cast<char *>(analysis_stringa.c_str()), "/");
    if (controller_map[res] != nullptr) {
      if (strcmp(res, "auth") == 0)
        return AuthController::getInstance();
      else if (strcmp(res, "status") == 0)
        return StatusController::getInstance();
      else if (strcmp(res, "chunk") == 0)
          return ChunkController::getInstance();
      else if (strcmp(res, "file") == 0)
          return FileController::getInstance();
      else if (strcmp(res, "test") == 0)
          return TestController::getInstance();
    }
    throw ControllerNotRetrievable();
  }
};
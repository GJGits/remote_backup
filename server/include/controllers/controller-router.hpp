#pragma once
#include "../exceptions/exceptions.hpp"
#include "auth-controller.hpp"
#include "chunk-controller.hpp"
#include "controller.hpp"
#include "file-controller.hpp"
#include "status-controller.hpp"
#include "test-controller.hpp"

class ControllerRouter : public Singleton<ControllerRouter> {
private:
  friend class Singleton;
  ControllerRouter() {}

public:
  std::shared_ptr<Controller> getController(const std::string &key) {

    std::string analysis_stringa(key);
    char *res = std::strtok(const_cast<char *>(analysis_stringa.c_str()), "/");

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
    throw WrongRquestFormat();
  }
};
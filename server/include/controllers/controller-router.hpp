#pragma once
#include "auth-controller.hpp"
#include "controller.hpp"
#include "status-controller.hpp"
#include "chunk-controller.hpp"
#include "../exceptions/exceptions.hpp"


class ControllerRouter {
private:
  inline static std::unordered_map<std::string, Controller *> controller_map{};
  inline static ControllerRouter *instance = nullptr;

public:

  static Controller * getController(const std::string &key) {
    // se mappa non inizializzata
    if (instance == nullptr) {
      instance = new ControllerRouter();
      controller_map["auth"] = AuthController::getInstance();
      controller_map["status"] = StatusController::getInstance();
      controller_map["chunk"] = ChunkController::getInstance();

        // todo: inizializza
    }
    // qui map ok, recupero controller
    // todo: recupera controller qui:
    std::string analysis_stringa(key);
    char *res = std::strtok(const_cast<char *>(analysis_stringa.c_str()), "/");
    std::clog << key << "\n";
    if (controller_map[res] != nullptr) {
      if (strcmp(res, "auth") == 0)
        return AuthController::getInstance();
      else if (strcmp(res, "status") == 0)
        return StatusController::getInstance();
      else if (strcmp(res, "Chunk") == 0)
          return ChunkController::getInstance();
    }
    throw ControllerNotRetrievable();
  }
};
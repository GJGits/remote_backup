#include "../../include/http/rest_client.hpp"

std::shared_ptr<RestClient> RestClient::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<RestClient>{new RestClient()};
  }
  return instance;
}


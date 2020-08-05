#pragma once
#include "../common/json.hpp"
#include <string>

// for convenience
using json = nlohmann::json;

class JSONSerializable {

protected:
  json json_object;

public:
  const virtual std::string serialize() = 0;
  const virtual void deserialize(const std::string &json_string) = 0;
};
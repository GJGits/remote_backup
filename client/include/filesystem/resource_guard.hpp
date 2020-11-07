#pragma once

#include <filesystem>
#include <fstream>

#include "../common/json.hpp"

using json = nlohmann::json;

class resource_guard {
private:
void reset_struct();
public:
  resource_guard();
  ~resource_guard();
};
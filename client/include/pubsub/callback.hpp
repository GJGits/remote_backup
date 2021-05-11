#pragma once

#include "message.hpp"
#include <functional>

enum PRIORITY { LOW, HIGH };

class Callback {
private:
  PRIORITY priority;
  std::function<void(const Message &)> call;

public:
  Callback(PRIORITY priority, const std::function<void(const Message &)> &call)
      : priority{priority}, call{call} {}
  std::function<void(const Message &)> get_call() const { return call; }
  PRIORITY get_priority() const { return priority; }
};
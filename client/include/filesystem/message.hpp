#pragma once

#include <string>
#include <vector>

enum MESSAGE_CODE { UPLOAD, PRUNING, REMOVE, NEW_ENTRY, STOP };

class Message {
private:
  MESSAGE_CODE code;
  std::vector<std::string> arguments;
  bool struct_handled, sync_handled;

public:
  Message(MESSAGE_CODE code, std::vector<std::string> arguments)
      : code{code}, arguments{std::move(arguments)} {}

  bool is_sync_handled() { return sync_handled; }
  bool is_struct_handled() { return struct_handled; }
  void set_sync(bool flag) { sync_handled = flag; }
  void set_struct(bool flag) { struct_handled = flag; }
  MESSAGE_CODE getCode() const { return code; }
  std::vector<std::string> getArguments() const { return arguments; }
};
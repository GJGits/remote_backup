#pragma once

#include "../common/json.hpp"
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

enum MESSAGE_CODE {
  UPLOAD,
  BULK_UPLOAD,
  PRUNING,
  REMOVE,
  NEW_ENTRY,
  UPDATE_PATH,
  FILE_MOVED,
  ADD_CHUNK,
  BULK_DELETE,
  EXPAND,
  INCREASE_COUNTER,
  STOP
};

class Message {
protected:
  MESSAGE_CODE code;
  int type;

public:
  Message(MESSAGE_CODE code, int type) : code{code}, type{type} {}
  virtual ~Message() {}
  virtual MESSAGE_CODE getCode() const = 0;
  virtual int getType() const = 0;
};

class StructMessage : public Message {
private:
  json argument;

public:
  StructMessage(MESSAGE_CODE code) : Message(code, 0) {}
  StructMessage(MESSAGE_CODE code, json arg) : Message(code, 0) {
    this->argument = arg;
  }
  MESSAGE_CODE getCode() const { return code; }
  int getType() const { return type; }
  json getArgument() const { return argument; }
};

class SyncMessage : public Message {
private:
  std::vector<std::string> argument;

public:
  SyncMessage(MESSAGE_CODE code) : Message(code, 1) {}
  SyncMessage(MESSAGE_CODE code, std::vector<std::string> argument)
      : Message(code, 1), argument{argument} {}
  MESSAGE_CODE getCode() const { return code; }
  int getType() const { return type; }
  std::vector<std::string> getArgument() const { return argument; }
};

class SignalMessage : public Message {
public:
  SignalMessage(MESSAGE_CODE code) : Message(code, -1) {}
  MESSAGE_CODE getCode() const { return code; }
  int getType() const { return type; }
};
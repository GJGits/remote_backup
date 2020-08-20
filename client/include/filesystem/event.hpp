#pragma once
#include <string>

enum EVENT_TYPE { CREATE, EXPAND, MOVED, PRUNING, RENAME, DELETE };

class Event {
private:
  EVENT_TYPE type;
  std::string argument1;
  std::string argument2;

public:
  Event(EVENT_TYPE type) : type{type} {}
  Event(EVENT_TYPE type, const std::string &argument1)
      : type{type}, argument1{std::move(argument1)} {}
  Event(EVENT_TYPE type, const std::string &argument1,
        const std::string &argument2)
      : type{type}, argument1{std::move(argument1)}, argument2{std::move(
                                                         argument2)} {}

  EVENT_TYPE getType() const { return type; }
  std::string getArgument1() const { return argument1; }
  std::string getArgument2() const { return argument2; }
};
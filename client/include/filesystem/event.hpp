#include <string>

enum EVENT_TYPE { CREATE, EXPAND, PRUNING, RENAME, DELETE };

class Event {
private:
  EVENT_TYPE type;
  std::string argument;

public:
  Event(EVENT_TYPE type, const std::string &argument)
      : type{type}, argument{std::move(argument)} {}

  EVENT_TYPE getType() const { return type; }
  std::string getArgument() const { return argument; }
};
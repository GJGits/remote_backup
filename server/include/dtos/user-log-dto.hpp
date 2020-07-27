#include <string>

class UserLogDTO {
private:
  std::string username;
  std::string password;

public:
  UserLogDTO(std::string username, std::string password);
};

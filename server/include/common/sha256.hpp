#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <openssl/sha.h>

class Sha256 {
private:
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  Sha256() { SHA256_Init(&sha256); }
  static Sha256 *instance;

public:
  Sha256(const Sha256 &) = delete;
  Sha256(Sha256 &&) = delete;
  Sha256 &operator=(const Sha256 &) = delete;
  Sha256 &operator=(Sha256 &&) = delete;

  static std::string getSha256(const std::string message) {
    if (instance == nullptr) {
      instance = new Sha256();
    }
    SHA256_Update(&instance->sha256, message.c_str(), message.size());
    SHA256_Final(instance.hash, &instance->sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      ss << hex << setw(2) << setfill('0') << (int)instance.hash[i];
    }
    return ss.str();
  }
};

Sha256 * Sha256::instance = nullptr;

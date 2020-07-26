#pragma once
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
  //static Sha256 *instance;

public:
  Sha256(const Sha256 &) = delete;
  Sha256(Sha256 &&) = delete;
  Sha256 &operator=(const Sha256 &) = delete;
  Sha256 &operator=(Sha256 &&) = delete;

  static std::string getSha256(const std::string message) {
    /*
    if (instance == nullptr) {
      instance = new Sha256();
    }
    */
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message.c_str(), message.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      ss << std::hex << std::setw(2) << std::setfill('0')
         << (int)hash[i];
    }
    return ss.str();
  }
};

//Sha256 * Sha256::instance = nullptr;
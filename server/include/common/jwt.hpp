#pragma once

#include "base64.hpp"
#include "hmac-sha256.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <ctime>

class JWT {

private:
  static JWT *getInstance() {
    if (instance == nullptr) {
      instance = new JWT{};
      instance->token_header = macaron::Base64::Encode(
          std::string{"\"alg\": \"HS256\",\"typ\": \"JWT\"}\"}"});
      std::string file_path{"../config/server-struct.json"};
      boost::property_tree::ptree pt;
      boost::property_tree::read_json(file_path, pt);
      boost::property_tree::ptree token_config = pt.get_child("token-conf");
      instance->secret = token_config.get<std::string>("secret");
      instance->expiration = token_config.get<int>("expiration");
      
    }
    return instance;
  }

  std::string token_header;
  inline static JWT *instance = nullptr;
  std::string secret;
  int expiration;

public:
  static std::string generateToken(const std::string &subscriber) {
    std::string payload = macaron::Base64::Encode(
        "{\"sub\":\"" + subscriber +
        "\", \"exp\":" + std::to_string(getInstance()->expiration + std::time(nullptr)) + "}");
    std::string to_sign{getInstance()->token_header + "." + payload};
    const unsigned char *text = (const unsigned char *)to_sign.c_str();
    int text_len = (int)to_sign.size();
    char kdigest[65];
    memset(kdigest, '\0', 65);
    HmacSha256::hmac_sha256(
        text, text_len, (const unsigned char *)getInstance()->secret.c_str(),
        (int)getInstance()->secret.size(), kdigest);
    std::string sign = macaron::Base64::Encode(std::string{kdigest});
    return getInstance()->token_header + "." + payload + "." + sign;
  }
};
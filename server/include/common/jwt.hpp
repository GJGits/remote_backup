#pragma once

#include "base64.hpp"
#include "hmac-sha256.hpp"
#include "json.hpp"
#include "utility.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ctime>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using json = nlohmann::json;
inline static std::regex time_rgx{"^\\d+$"};

class JWT {

private:
  std::string token_header;
  inline static JWT *instance = nullptr;
  std::string secret;
  int expiration;

  static JWT *getInstance() {
    if (instance == nullptr) {
      instance = new JWT{};
      json config;
      std::ifstream i("../config/server-struct.json");
      i >> config;
      json jwt_header = {{"alg", "HS256"}, {"typ", "JWT"}};
      std::clog << "header dump = " << jwt_header.dump() << "\n";
      instance->token_header = macaron::Base64::Encode(jwt_header.dump());
      instance->secret = config["token-conf"]["secret"];
      instance->expiration = config["token-conf"]["expiration"];
    }
    return instance;
  }

public:
  static int getExpiration() { return getInstance()->expiration; }

  static std::string generateToken(const std::string &subscriber,
                                   int expiration) {
    json jwt_payload = {{"sub", subscriber}, {"exp", expiration}};
    std::string payload = macaron::Base64::Encode(jwt_payload.dump());
    std::string to_sign{getInstance()->token_header + "." + payload};
    const unsigned char *text = (const unsigned char *)to_sign.c_str();
    int text_len = (int)to_sign.size();
    char kdigest[65];
    memset(kdigest, '\0', 65);
    HmacSha256::hmac_sha256(
        text, text_len, (const unsigned char *)getInstance()->secret.c_str(),
        (int)getInstance()->secret.size(), kdigest);
    json jwt_sign; 
    jwt_sign["sign"] = macaron::Base64::Encode(std::string{kdigest});
    std::clog << "sign = " << jwt_sign["sign"] << "\n";
    std::string sign = macaron::Base64::Encode(jwt_sign.dump());
    return getInstance()->token_header + "." + payload + "." + sign;
  }

  static bool validateToken(const http::server::request &req) {
    std::clog << "e0\n";
    for (http::server::header h : req.headers) {
      if (h.name.compare("Authorization") == 0) {
        std::vector<std::string> tokens = Utility::split(h.value, ' ');
        std::clog << "token = " << tokens[1] << "\n";
        if (tokens.size() == 2 && tokens[0].compare("Bearer") == 0) {
          std::clog << "e1\n";
          std::string token = tokens[1];
          std::vector<std::string> token_parts = Utility::split(token, '.');
          if (token_parts.size() == 3) {
            // todo: verificare che le str siano codificabili in base64
            std::string jwt_header_str =
                macaron::Base64::Decode(token_parts[0]);
            std::string jwt_payload_str =
                macaron::Base64::Decode(token_parts[1]);
            std::string jwt_sign_str = macaron::Base64::Decode(token_parts[2]);
            std::clog << "head = " << jwt_header_str
                      << ", pay = " << jwt_payload_str << "\n";
            std::clog << "head = " << token_parts[0]
                      << ", pay = " << token_parts[1] << "\n";
            // todo: check sul formato di jwt_xxx_str
            json jwt_header = json::parse(jwt_header_str);
            json jwt_payload = json::parse(jwt_payload_str);
            json jwt_sign = json::parse(jwt_sign_str);
            std::smatch match;
            std::string ex_str = std::to_string(jwt_payload["exp"].get<int>());
            if (std::regex_match(ex_str, match, time_rgx)) {
              std::clog << "e3\n";
              int exp = jwt_payload["exp"];
              std::string sign = jwt_sign["sign"];
              if (exp >= std::time(nullptr)) {
                std::clog << "e4\n";
                std::string token_calc = generateToken(jwt_payload["sub"], exp);
                std::clog << "token calc = " << token_calc << "\n";
                json sign_cal =
                    json::parse(macaron::Base64::Decode(Utility::split(
                        generateToken(jwt_payload["sub"], exp), '.')[2]));
                if (sign.compare(sign_cal["sign"]) == 0)
                  return true;
              }
            }
          }
        }
      }
    }
    return false;
  }
};
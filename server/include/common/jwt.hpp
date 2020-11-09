#pragma once

#include "base64.hpp"
#include "hmac-sha256.hpp"
#include "json.hpp"
#include "utility.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <tuple>
#include <unordered_map>
#include "../http/request.hpp"
#include "duration.hpp"
#include "../dtos/subject.hpp"
#include "jwt_cache_entry.hpp"

using json = nlohmann::json;
inline static std::regex time_rgx{"^\\d+$"};

class JWT {

private:
  std::unordered_map<std::string, JWTCacheEntry> tok_cache;
  inline static std::regex raw_tok_rgx{
      "^[a-zA-Z0-9+=\\/]+\\.[a-zA-Z0-9+=\\/]+\\.[a-zA-Z0-9+=\\/]+$"};
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
      instance->token_header = macaron::Base64::Encode(jwt_header.dump());
      instance->secret = config["token-conf"]["secret"];
      instance->expiration = config["token-conf"]["expiration"];
    }
    return instance;
  }

public:
  static int getExpiration() { return getInstance()->expiration; }

  static std::string generateToken(const Subject &sub, int expiration) {
    json jwt_payload = {{"sub", sub.get_sub()},
                        {"db", sub.get_db_id()},
                        {"device_id", sub.get_device_id()},
                        {"exp", expiration}};
    std::string payload = macaron::Base64::Encode(jwt_payload.dump());
    std::string to_sign{getInstance()->token_header + "." + payload};
    const unsigned char *text = (const unsigned char *)to_sign.c_str();
    int text_len = (int)to_sign.size();
    char kdigest[65];
    memset(kdigest, '\0', 65);
    HmacSha256::hmac_sha256(
        text, text_len, (const unsigned char *)getInstance()->secret.c_str(),
        (int)getInstance()->secret.size(), kdigest);
    json jwt_sign{{"sign", macaron::Base64::Encode(std::string{kdigest})}};
    std::string sign = macaron::Base64::Encode(jwt_sign.dump());
    return getInstance()->token_header + "." + payload + "." + sign;
  }

  static Subject validateToken(const http::server::request &req) {
    DurationLogger logger{"VALIDATION"};
    for (http::server::header h : req.headers) {
      if (h.name.compare("Authorization") == 0) {
        std::vector<std::string> tokens = Utility::split(h.value, ' ');
        if (tokens.size() == 2 && tokens[0].compare("Bearer") == 0) {
          std::string token = tokens[1];
          // inizialmente cerco nella cache
          if (getInstance()->tok_cache.find(token) !=
              getInstance()->tok_cache.end()) {
            JWTCacheEntry entry = getInstance()->tok_cache[token];
            if (entry.get_exp() >= std::time(nullptr)) {
              return entry.get_sub();
            } else
              getInstance()->tok_cache.erase(token);
          }
          // altrimenti procedo normalmente

          std::vector<std::string> token_parts = Utility::split(token, '.');
          if (token_parts.size() == 3) {
            json payload = json::parse(macaron::Base64::Decode(token_parts[1]));
            Subject sbj{payload["sub"], (size_t)payload["db"].get<int>(),
                        (size_t)payload["device_id"].get<int>()};
            int exp = payload["exp"];
            std::string sign =
                json::parse(macaron::Base64::Decode(token_parts[2]))["sign"];
            std::string sign_calc = json::parse(macaron::Base64::Decode(
                Utility::split(generateToken(sbj, exp), '.')[2]))["sign"];
            if (sign.compare(sign_calc) == 0) {
              JWTCacheEntry ce{sbj, exp};
              // todo: valutare politica di replace per evitare di intasare
              // memoria
              getInstance()->tok_cache[token] = ce;
              return sbj;
            }
          }
        }
      }
    }
    throw InvalidJWT();
  }
};
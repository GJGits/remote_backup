#pragma once

#include <string>

#include "../dtos/subject.hpp"

class JWTCacheEntry {
private:
  Subject sub;
  int exp;

public:
  JWTCacheEntry(){}
  JWTCacheEntry(const Subject &sub, int exp) : sub{sub}, exp{exp} {}
  Subject get_sub() const { return sub; }
  int get_exp() const { return exp; }
};
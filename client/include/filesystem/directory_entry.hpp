#pragma once
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "../common/json.hpp"




using namespace std::chrono_literals;

class DirEntry {
private:
  std::string path;
  json entry;

public:
  DirEntry(const std::string &path) : path{path} {
    entry["path"] = path;

  }

  json getEntry() {return entry;}

};

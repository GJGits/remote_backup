#pragma once
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <cstring>

class Logger {
public:
  static std::string log(const std::string &message) {
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string ret_str {std::string{std::strtok(std::ctime(&time), "\n")} + std::string{"\r\r: "} + message};
    return std::string{std::strtok(std::ctime(&time), "\n")} + std::string{" : "} + message;
  }
};

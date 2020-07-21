#pragma once
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>

class Logger {
public:
  static std::string log(const std::string &message) {
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return std::string{std::ctime(&time)} + std::string{"\r\r: "} + message;
  }
};
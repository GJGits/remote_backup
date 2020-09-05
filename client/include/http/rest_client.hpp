#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../common/base64.hpp"
#include "../common/json.hpp"
#include "up_worker.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using json = nlohmann::json;

class RestClient {

private:
  json config;
  static inline std::shared_ptr<RestClient> instance{nullptr};

  RestClient(){}
  void read_info();

public:
  static std::shared_ptr<RestClient> getInstance();
  void post_chunk(json &chk_info, std::shared_ptr<char[]> &chunk, size_t size, size_t nchunks);
  void put_chunk(json &chk_info, std::shared_ptr<char[]> &chunk, size_t size, size_t nchunks);
  void delete_chunk(json &chk_info, size_t size);
  void get_chunk();
  void delete_file(std::string &path);
  void get_status();
  void get_status_file();
};
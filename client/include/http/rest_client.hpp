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

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using json = nlohmann::json;

class RestClient {

private:
  static inline std::shared_ptr<RestClient> instance{nullptr};

  RestClient(){}

public:
  static std::shared_ptr<RestClient> getInstance();
  void post_chunk(std::shared_ptr<char[]> &chunk);
  void put_chunk(std::shared_ptr<char[]> &chunk);
  void delete_chunk(json &chk_info);
  void get_chunk();
  void delete_file();
  void get_status();
  void get_status_file();
};
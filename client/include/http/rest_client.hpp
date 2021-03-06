#pragma once

#include <ctime>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../common/constants.hpp"
#include "../exceptions/exceptions.hpp"
#include "../common/singleton.hpp"
#include "../common/base64.hpp"
#include "../common/json.hpp"
#include "../http/http_client.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using json = nlohmann::json;

class RestClient : public Singleton<RestClient>{

private:
  friend class Singleton;
  json config;
  http::request<http::vector_body<char>> post_prototype;
  http::request<http::vector_body<char>> put_prototype;
  http::request<http::vector_body<char>> get_prototype;
  http::request<http::vector_body<char>> delete_prototype;
  void fill_headers(http::request<http::vector_body<char>> &req,
                    size_t size = 0);
  RestClient();

public:
  ~RestClient();
  void read_info();
  void post_chunk(std::tuple<std::shared_ptr<char[]>, size_t> &chunk,
                            const std::string &target);
  std::vector<char> get_chunk(const std::string &target);
  void delete_file(const std::string &path);
  json get_status_list(size_t page, size_t last_check);
};
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

#include "../common/singleton.hpp"
#include "../common/base64.hpp"
#include "../common/json.hpp"
#include "../filesystem/file_entry.hpp"
#include "../http/http_client.hpp"
#include "../filesystem/sync_structure.hpp"

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
  void read_info();
  void fill_headers(http::request<http::vector_body<char>> &req,
                    size_t size = 0);
  RestClient();

public:
  void post_chunk(std::tuple<std::shared_ptr<char[]>, size_t> &chunk,
                  json &jentry);
  std::vector<char> get_chunk(const json &chunk_info);
  void delete_file(std::string &path);
  json get_status_list(size_t page);
};
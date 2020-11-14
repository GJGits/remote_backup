#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../common/base64.hpp"
#include "../common/duration.hpp"
#include "../common/json.hpp"
#include "../common/singleton.hpp"
#include "../exceptions/exceptions.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using json = nlohmann::json;

class HTTPClient : public Singleton<HTTPClient> {

private:
  friend class Singleton;
  const char *host = "remote_backup_nginx-server_1";
  const char *port = "80";
  std::mutex mu;
  net::io_context ioc;
  tcp::resolver resolver{ioc};
  beast::tcp_stream stream{ioc};
  boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> results;

  HTTPClient();

  void send(beast::tcp_stream &stream,
            const http::request<http::vector_body<char>> &request);

  http::response<http::vector_body<char>> read(beast::tcp_stream &stream);

public:
 ~HTTPClient();
  void up_request(const http::request<http::vector_body<char>> &req);

  json get_json(const http::request<http::vector_body<char>> &req);

  std::vector<char>
  get_binary(const http::request<http::vector_body<char>> &req);
};
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
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../common/base64.hpp"
#include "../common/duration.hpp"
#include "../common/json.hpp"
#include "../pubsub/broker.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using json = nlohmann::json;

class HTTPClient {

private:
  static inline std::shared_ptr<HTTPClient> instance{nullptr};
  const char *host = "remote_backup_nginx-server_1";
  const char *port = "80";
  net::io_context ioc;
  tcp::resolver resolver{ioc};
  beast::tcp_stream stream{ioc};
  boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> results;

public:
  static std::shared_ptr<HTTPClient> getIstance() {
    if (instance.get() == nullptr) {
      instance = std::shared_ptr<HTTPClient>{new HTTPClient{}};
      instance->results =
          instance->resolver.resolve(instance->host, instance->port);
    }
    return instance;
  }

  void post(const http::request<http::vector_body<char>> &req) {
    stream.connect(results);
    send(stream, req);
    http::response<http::dynamic_body> res = read(stream);
    std::clog << "response: " << res << "\n";
  }

  void delete_(const http::request<http::vector_body<char>> &req) {
    stream.connect(results);
    send(stream, req);
    http::response<http::dynamic_body> res = read(stream);
    std::clog << "response: " << res << "\n";
  }

  json get_json(const http::request<http::vector_body<char>> &req) {
    stream.connect(results);
    send(stream, req);
    http::response<http::dynamic_body> res = read(stream);
    return json::parse(req.body());
  }

  std::vector<char>
  get_binary(const http::request<http::vector_body<char>> &req) {
    stream.connect(results);
    send(stream, req);
    http::response<http::dynamic_body> res = read(stream);
    return req.body();
  }

  void send(beast::tcp_stream &stream,
            const http::request<http::vector_body<char>> &request) {
    DurationLogger{"SEND"};
    http::write(stream, request);
  }

  http::response<http::dynamic_body> read(beast::tcp_stream &stream) {
    DurationLogger{"READ"};
    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;
    // Declare a container to hold the response
    http::response<http::dynamic_body> res;
    // Receive the HTTP response
    http::read(stream, buffer, res);
    return res;
  }
};
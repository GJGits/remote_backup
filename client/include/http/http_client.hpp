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
#include "../common/singleton.hpp"
#include "../pubsub/broker.hpp"

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

  HTTPClient() {
    try {
      this->results = resolver.resolve(host, port);
      std::clog << "http 1\n";
    } catch (const boost::exception &e) {
      throw ConnectionNotAvaible();
    }
  }

public:

  void up_request(const http::request<http::vector_body<char>> &req) {
    beast::tcp_stream str_temp{ioc};
    uint8_t retry = 3;
    while (retry) {
      try {
        DurationLogger logger{"COMPLETE REQUEST"};
        str_temp.connect(results);
        usleep(10000);
        send(str_temp, req);
        http::response<http::vector_body<char>> res = read(str_temp);
        uint32_t result = res.result_int();
        if (result == 200)
          return;
        if (result == 400) {
          // todo: gestisci
        }
        retry--;
      } catch (const boost::exception &e) {
        throw ConnectionNotAvaible();
      }
    }
    throw ConnectionNotAvaible();
  }


  json get_json(const http::request<http::vector_body<char>> &req) {
    stream.connect(results);
    send(stream, req);
    http::response<http::vector_body<char>> res = read(stream);
    std::string dump{res.body().begin(), res.body().end()};
    return json::parse(dump);
  }

  std::vector<char>
  get_binary(const http::request<http::vector_body<char>> &req) {
    stream.connect(results);
    send(stream, req);
    http::response<http::vector_body<char>> res = read(stream);
    return res.body();
  }

  void send(beast::tcp_stream &stream,
            const http::request<http::vector_body<char>> &request) {
    DurationLogger{"SEND"};
    http::write(stream, request);
  }

  http::response<http::vector_body<char>> read(beast::tcp_stream &stream) {
    DurationLogger{"READ"};
    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;
    // Declare a container to hold the response
    http::response<http::vector_body<char>> res;
    // Receive the HTTP response
    http::read(stream, buffer, res);
    return res;
  }
};
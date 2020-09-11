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
#include <tuple>

#include "../common/base64.hpp"
#include "../common/duration.hpp"
#include "../common/json.hpp"
#include "../pubsub/broker.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using json = nlohmann::json;

class UpWorker {
private:
  bool is_running;
  std::vector<std::thread> workers;
  std::mutex m;
  std::condition_variable cv;
  std::queue<std::tuple<http::request<http::vector_body<char>>, Message>> requests;
  static inline std::shared_ptr<UpWorker> instance{nullptr};
  const char *host = "remote_backup_nginx-server_1";
  const char *port = "80";
  size_t job_count;

  UpWorker()
      : is_running{true}, job_count{0} {}

public:
  static std::shared_ptr<UpWorker> getIstance();
  void run();
  void push_request(const std::tuple<http::request<http::vector_body<char>>, const Message> &request);
  void send(beast::tcp_stream& stream, http::request<http::vector_body<char>> &request);
  http::response<http::dynamic_body> read(beast::tcp_stream &stream);
  ~UpWorker();
};
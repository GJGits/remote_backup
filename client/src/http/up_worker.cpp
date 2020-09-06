#include "../../include/http/up_worker.hpp"

std::shared_ptr<UpWorker> UpWorker::getIstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<UpWorker>{new UpWorker()};
    instance->run();
  }
  return instance;
}

void UpWorker::run() {
  // const char *host = "remote_backup_nginx-server_1";
  // const char *port = "80";
  // net::io_context ioc;
  // tcp::resolver resolver(ioc);
  // beast::tcp_stream stream(ioc);
  // auto const results = resolver.resolve(host, port);
  for (size_t i = 0; i < 2; i++) {
    workers.emplace_back([&]() {
      std::shared_ptr<Broker> broker = Broker::getInstance();
      while (is_running) {
        http::request<http::vector_body<char>> req;
        {
          std::unique_lock lk{m};
          if (!requests.empty()) {
            req = std::move(requests.front());
            requests.pop();
          } else {
            broker->publish(TOPIC::UP_EMPTY, Message{});
            cv.wait(lk, [&]() { return !requests.empty() || !is_running; });
            continue;
          }
        }
        // stream.connect(results);
        // req.set(http::field::host, host);
        // http::write(stream, req);
      }
    });
  }
}

void UpWorker::push_request(
    const http::request<http::vector_body<char>> &request) {
  std::unique_lock lk{m};
  requests.push(request);
  cv.notify_one();
}

UpWorker::~UpWorker() {
  is_running = false;
  for (size_t i = 0; i < 2; i++) {
    workers[i].join();
  }
}
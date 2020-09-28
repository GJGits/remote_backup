#include "../../include/http/down_worker.hpp"

std::shared_ptr<DownWorker> DownWorker::getIstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<DownWorker>{new DownWorker()};
    instance->run();
  }
  return instance;
}

void DownWorker::run() {
  for (size_t i = 0; i < 2; i++) {
    workers.emplace_back([&]() {
      net::io_context ioc;
      tcp::resolver resolver(ioc);
      beast::tcp_stream stream(ioc);
      auto const results = resolver.resolve(host, port);
      std::shared_ptr<Broker> broker = Broker::getInstance();
      while (is_running) {
        http::request<http::vector_body<char>> req;
        Message mex;
        {
          std::unique_lock lk{m};
          if (!requests.empty()) {
            std::tuple pair = std::move(requests.front());
            req = std::get<0>(pair);
            mex = std::get<1>(pair);
            requests.pop();
          } else {
            cv.wait(lk, [&]() { return !requests.empty() || !is_running; });
            continue;
          }
        }
        stream.connect(results);
        send(stream, req);
        http::response<http::vector_body<char>> res = read(stream);
        std::clog << "response: " << res << "\n";
        if (res.result_int() == 200) {
          json list = json::parse(res.body());
          mex.set_content(list);
          broker->publish(mex);
        }

        {
          std::unique_lock lk{m};
          job_count--;
          if (job_count == 0)
            broker->publish(TOPIC::DOWN_EMPTY);
        }
      }
    });
  }
}

void DownWorker::push_request(
    const std::tuple<http::request<http::vector_body<char>>, const Message>
        &request) {
  std::unique_lock lk{m};
  job_count++;
  requests.push(request);
  cv.notify_one();
}

void DownWorker::send(beast::tcp_stream &stream,
                      http::request<http::vector_body<char>> &request) {
  DurationLogger{"SEND"};
  http::write(stream, request);
}

http::response<http::vector_body<char>>
DownWorker::read(beast::tcp_stream &stream) {
  DurationLogger{"READ"};
  // This buffer is used for reading and must be persisted
  beast::flat_buffer buffer;
  // Declare a container to hold the response
  http::response<http::vector_body<char>> res;
  // Receive the HTTP response
  http::read(stream, buffer, res);
  return res;
}

DownWorker::~DownWorker() {
  is_running = false;
  for (size_t i = 0; i < 2; i++) {
    workers[i].join();
  }
}

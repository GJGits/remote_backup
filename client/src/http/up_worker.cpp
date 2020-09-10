#include "../../include/http/up_worker.hpp"

std::shared_ptr<UpWorker> UpWorker::getIstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<UpWorker>{new UpWorker()};
    instance->run();
  }
  return instance;
}

void UpWorker::run() {
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
        read(stream);
        broker->publish(mex);

        {
          std::unique_lock lk{m};
          job_count--;
          if (job_count == 0)
            broker->publish(TOPIC::UP_EMPTY);
        }
      }
    });
  }
}

void UpWorker::push_request(
    const std::tuple<http::request<http::vector_body<char>>, const Message>
        &request) {
  std::unique_lock lk{m};
  job_count++;
  requests.push(request);
  cv.notify_one();
}

void UpWorker::send(beast::tcp_stream &stream,
                    http::request<http::vector_body<char>> &request) {
  DurationLogger{"SEND"};
  http::write(stream, request);
}

void UpWorker::read(beast::tcp_stream &stream) {
  DurationLogger{"READ"};
  sleep(1);
}

UpWorker::~UpWorker() {
  is_running = false;
  for (size_t i = 0; i < 2; i++) {
    workers[i].join();
  }
}
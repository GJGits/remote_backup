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
      std::shared_ptr<Broker> broker = Broker::getInstance();
      while (is_running) {
        DurationLogger{"UP_JOB"};
        http::request<http::vector_body<char>> req;
        {
          std::unique_lock lk{m};
          if (!requests.empty()) {
            req = std::move(requests.front());
            requests.pop();
          } else {
            broker->publish(Message{TOPIC::UP_EMPTY});
            cv.wait(lk, [&]() { return !requests.empty() || !is_running; });
            continue;
          }
        }
        // fake upload time for a request plus response todo: delete
        send(req);
        read();
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

void UpWorker::send(http::request<http::vector_body<char>> &request) {
  // auto const results = resolver.resolve(host, port);
  // stream.connect(results);
  // req.set(http::field::host, host);
  // http::write(stream, req);
  sleep(1);
}

void UpWorker::read() { sleep(1); }

UpWorker::~UpWorker() {
  is_running = false;
  for (size_t i = 0; i < 2; i++) {
    workers[i].join();
  }
}
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
        // fake upload time for a request plus response todo: delete
        // send(req);
        // read();

        auto const results = resolver.resolve(host, port);
        stream.connect(results);
        req.set(http::field::host, host);
        for (auto it = req.body().begin(); it != req.body().end(); it++) {
          std::clog << "char body: " << *it << "\n";
        }
        http::write(stream, req);
        read();

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

void UpWorker::send(http::request<http::vector_body<char>> &request) {
  DurationLogger{"SEND"};
  // auto const results = resolver.resolve(host, port);
  // stream.connect(results);
  // req.set(http::field::host, host);
  // http::write(stream, req);
  sleep(1);
}

void UpWorker::read() {
  DurationLogger{"READ"};
  sleep(1);
}

UpWorker::~UpWorker() {
  is_running = false;
  for (size_t i = 0; i < 2; i++) {
    workers[i].join();
  }
}
#include "../../include/http/http_client.hpp"

HTTPClient::HTTPClient() {
  try {
    this->results = resolver.resolve(host, port);
  } catch (const boost::exception &e) {
    throw ConnectionNotAvaible();
  }
}

void HTTPClient::up_request(const http::request<http::vector_body<char>> &req) {
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

json HTTPClient::get_json(const http::request<http::vector_body<char>> &req) {
  stream.connect(results);
  send(stream, req);
  http::response<http::vector_body<char>> res = read(stream);
  std::string dump{res.body().begin(), res.body().end()};
  return json::parse(dump);
}

std::vector<char>
HTTPClient::get_binary(const http::request<http::vector_body<char>> &req) {
  stream.connect(results);
  send(stream, req);
  http::response<http::vector_body<char>> res = read(stream);
  return res.body();
}

void HTTPClient::send(beast::tcp_stream &stream,
          const http::request<http::vector_body<char>> &request) {
  DurationLogger{"SEND"};
  http::write(stream, request);
}

http::response<http::vector_body<char>> HTTPClient::read(beast::tcp_stream &stream) {
  DurationLogger{"READ"};
  // This buffer is used for reading and must be persisted
  beast::flat_buffer buffer;
  // Declare a container to hold the response
  http::response<http::vector_body<char>> res;
  // Receive the HTTP response
  http::read(stream, buffer, res);
  return res;
}
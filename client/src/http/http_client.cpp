#include "../../include/http/http_client.hpp"

HTTPClient::HTTPClient() {
  try {
    // se non riesco a risolvere nome DNS ho problemi di connessione
    // o il server non e' raggiungibile
    this->results = resolver.resolve(host, port);
    std::clog << "http_client init\n";
  } catch (const boost::exception &e) {
    throw ConnectionNotAvaible();
  }
}

HTTPClient::~HTTPClient() { std::clog << "http_client destroy...\n"; }

void HTTPClient::up_request(const http::request<http::vector_body<char>> &req) {

  try {
    DurationLogger logger{"COMPLETE REQUEST"};
    beast::tcp_stream str_temp{ioc};
    str_temp.connect(results);
    send(str_temp, req);
    http::response<http::vector_body<char>> res = read(str_temp);
    uint32_t result = res.result_int();
    if (result == 200)
      return;
    if (result == 401) {
      // autenticazione fallita
      throw AuthFailed();
    }
    if (result == 400 || result == 500 || result == 502 || result == 503 ||
        result == 504) {
      // server non raggiungibile
      std::string dump{res.body().begin(), res.body().end()};
      std::clog << "error: " << dump << "\n";
      throw ConnectionNotAvaible();
    }
  } catch (const boost::exception &e) {
    throw ConnectionNotAvaible();
  }
}

json HTTPClient::get_json(const http::request<http::vector_body<char>> &req) {

  try {
    beast::tcp_stream str_temp{ioc};
    str_temp.connect(results);
    send(str_temp, req);
    http::response<http::vector_body<char>> res = read(str_temp);
    uint32_t result = res.result_int();
    if (result == 200) {
      std::string dump{res.body().begin(), res.body().end()};
      return json::parse(dump);
    }
    if (result == 401) {
      // autenticazione fallita
      throw AuthFailed();
    }
    if (result == 502 || result == 503 || result == 504) {
      // server non raggiungibile
      throw ConnectionNotAvaible();
    }
  } catch (const boost::exception &e) {
    throw ConnectionNotAvaible();
  }
  return {};
}

std::vector<char>
HTTPClient::get_binary(const http::request<http::vector_body<char>> &req) {

  try {
    beast::tcp_stream str_temp{ioc};
    str_temp.connect(results);
    send(str_temp, req);
    http::response<http::vector_body<char>> res = read(str_temp);
    uint32_t result = res.result_int();
    if (result == 200) {
      return res.body();
    }
    if (result == 401) {
      // autenticazione fallita
      throw AuthFailed();
    }
    if (result == 502 || result == 503 || result == 504) {
      // server non raggiungibile
      throw ConnectionNotAvaible();
    }

  } catch (const boost::exception &e) {
    throw ConnectionNotAvaible();
  }
  return {};
}

void HTTPClient::send(beast::tcp_stream &stream,
                      const http::request<http::vector_body<char>> &request) {
  DurationLogger{"SEND"};
  http::write(stream, request);
}

http::response<http::vector_body<char>>
HTTPClient::read(beast::tcp_stream &stream) {
  DurationLogger{"READ"};
  // This buffer is used for reading and must be persisted
  beast::flat_buffer buffer;
  // Declare a container to hold the response
  http::response<http::vector_body<char>> res;
  // Receive the HTTP response
  http::read(stream, buffer, res);
  return res;
}

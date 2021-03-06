#include "../../include/http/rest_client.hpp"

RestClient::RestClient() {
  std::clog << "rest_client init\n";
  config = json::object();
  post_prototype = http::request<http::vector_body<char>>{http::verb::post,
                                                          "/some_target", 10};
  get_prototype = http::request<http::vector_body<char>>{http::verb::get,
                                                         "/some_target", 10};
  delete_prototype = http::request<http::vector_body<char>>{http::verb::delete_,
                                                            "/some_target", 10};
  fill_headers(post_prototype);
  fill_headers(get_prototype);
  fill_headers(delete_prototype);
}

RestClient::~RestClient() {
  std::clog << "rest_client destroy...\n";
}

void RestClient::read_info() {
  std::ifstream i(CLIENT_CONF);
  i.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  i >> config;
}

void RestClient::fill_headers(http::request<http::vector_body<char>> &req,
                              size_t size) {
  req.set(http::field::host, "remote_backup_nginx-server_1");
  req.set(http::field::content_length, std::to_string(size));
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
}

void RestClient::post_chunk(std::tuple<std::shared_ptr<char[]>, size_t> &chunk,
                            const std::string &target) {
  //DurationLogger log{"POST_CHUNK"};
  std::shared_ptr<HTTPClient> http_client = HTTPClient::getInstance();
  std::shared_ptr<char[]> buffer = std::get<0>(chunk);
  size_t size = std::get<1>(chunk);
  http::request<http::vector_body<char>> req{post_prototype};
  req.target("/chunk/" + target);
  req.set(http::field::content_length, std::to_string(size));
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  for (size_t i = 0; i < size; i++)
    req.body().push_back(buffer.get()[i]);

  http_client->up_request(req);
}

std::vector<char> RestClient::get_chunk(const std::string &target) {
  std::shared_ptr<HTTPClient> http_client = HTTPClient::getInstance();
  http::request<http::vector_body<char>> req{get_prototype};
  // chunk + id + file_path_base64
  req.target("/chunk/" + target);
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  return http_client->get_binary(req);
}

void RestClient::delete_file(const std::string &path) {
  std::shared_ptr<HTTPClient> http_client = HTTPClient::getInstance();
  http::request<http::vector_body<char>> req{delete_prototype};
  req.target("/file/" + macaron::Base64::Encode(path) + "/" +
             std::to_string((int)std::time(nullptr)));
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  http_client->up_request(req);
}

json RestClient::get_status_list(size_t page, size_t last_check) {
  std::shared_ptr<HTTPClient> http_client = HTTPClient::getInstance();
  http::request<http::vector_body<char>> req{get_prototype};
  req.target("/status/list/" + std::to_string(page) + "/" +
             std::to_string(last_check));
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  return http_client->get_json(req);
}

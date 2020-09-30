#include "../../include/http/rest_client.hpp"

RestClient::RestClient() {
  post_prototype = http::request<http::vector_body<char>>{http::verb::post,
                                                          "/some_target", 10};
  put_prototype = http::request<http::vector_body<char>>{http::verb::put,
                                                         "/some_target", 10};
  get_prototype = http::request<http::vector_body<char>>{http::verb::get,
                                                         "/some_target", 10};
  delete_prototype = http::request<http::vector_body<char>>{http::verb::delete_,
                                                            "/some_target", 10};
  read_info();
  fill_headers(post_prototype);
  fill_headers(put_prototype);
  fill_headers(get_prototype);
  fill_headers(delete_prototype);
}

std::shared_ptr<RestClient> RestClient::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<RestClient>{new RestClient{}};
  }
  instance->read_info();
  return instance;
}

void RestClient::read_info() {
  std::ifstream i("./config/client-conf.json");
  i >> config;
}

void RestClient::fill_headers(http::request<http::vector_body<char>> &req,
                              size_t size) {
  req.set(http::field::host, "remote_backup_nginx-server_1");
  req.set(http::field::content_length, std::to_string(size));
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
}

void RestClient::post_chunk(std::tuple<std::shared_ptr<char[]>, size_t> &chunk,
                            json &jentry) {
  std::shared_ptr<HTTPClient> http_client = HTTPClient::getIstance();
  std::shared_ptr<char[]> buffer = std::get<0>(chunk);
  http::request<http::vector_body<char>> req{post_prototype};
  size_t size = std::get<1>(chunk);
  req.target("/chunk/" + std::to_string(jentry["chunks"][0]["id"].get<int>()) +
             "/" + std::to_string(std::get<1>(chunk)) + "/" +
             std::string{jentry["chunks"][0]["hash"]} + "/" +
             std::to_string(jentry["nchunks"].get<int>()) + "/" +
             macaron::Base64::Encode(std::string{jentry["path"]}.substr(7)) +
             "/" + std::to_string(jentry["last_mod"].get<int>()));
  req.set(http::field::content_length, std::to_string(size));
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  std::move(buffer.get(), buffer.get() + size, std::back_inserter(req.body()));
  http_client->post(req);
}

void RestClient::get_chunk() {}

void RestClient::delete_file(std::string &path) {
  std::shared_ptr<HTTPClient> http_client = HTTPClient::getIstance();
  json jentry;
  jentry["path"] = path;
  http::request<http::vector_body<char>> req{delete_prototype};
  req.target("/file/" + macaron::Base64::Encode(path.substr(7)));
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  http_client->delete_(req);
}

json RestClient::get_status_list(size_t page) {
  std::shared_ptr<HTTPClient> http_client = HTTPClient::getIstance();
  http::request<http::vector_body<char>> req{get_prototype};
  req.target("/status/list/" + std::to_string(page));
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  return http_client->get_json(req);
}

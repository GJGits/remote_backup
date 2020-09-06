#include "../../include/http/rest_client.hpp"

std::shared_ptr<RestClient> RestClient::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<RestClient>{new RestClient()};
  }
  instance->read_info();
  return instance;
}

void RestClient::read_info() {
  std::ifstream i("./config/client-conf.json");
  i >> config;
}

void RestClient::post_chunk(json &chk_info, std::shared_ptr<char[]> &chunk,
                            size_t size, size_t nchunks) {
  std::shared_ptr<UpWorker> up_worker = UpWorker::getIstance();
  std::string user = config["username"];
  std::string token = config["token"];
  std::string path = chk_info["path"];
  std::string hash = chk_info["chunks"][0]["hash"];
  size_t chk_id = chk_info["chunks"][0]["id"];
  size_t timestamp = chk_info["last_mod"];
  http::request<http::vector_body<char>> req{
      http::verb::post,
      "/chunk/" + user + "/" + std::to_string(chk_id) + "/" +
          std::to_string(size) + "/" + hash + "/" + std::to_string(nchunks) +
          "/" + macaron::Base64::Encode(path) + std::to_string(timestamp),
      10};
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  up_worker->push_request(req);
}

void RestClient::put_chunk(json &chk_info, std::shared_ptr<char[]> &chunk,
                           size_t size, size_t nchunks) {
  std::shared_ptr<UpWorker> up_worker = UpWorker::getIstance();
  std::string user = config["username"];
  std::string token = config["token"];
  std::string path = chk_info["path"];
  std::string hash = chk_info["chunks"][0]["hash"];
  size_t chk_id = chk_info["chunks"][0]["id"];
  size_t timestamp = chk_info["last_mod"];
  http::request<http::vector_body<char>> req{
      http::verb::put,
      "/chunk/" + user + "/" + std::to_string(chk_id) + "/" +
          std::to_string(size) + "/" + hash + "/" + std::to_string(nchunks) +
          "/" + macaron::Base64::Encode(path) + "/" + std::to_string(timestamp),
      10};
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(http::field::authorization, "Barear " + token);
  up_worker->push_request(req);
}

void RestClient::delete_chunk(json &chk_info, size_t size) {
  std::shared_ptr<UpWorker> up_worker = UpWorker::getIstance();
  std::string user = config["username"];
  std::string token = config["token"];
  std::string path = chk_info["path"];
  size_t chk_id = chk_info["chunks"][0]["id"];
  http::request<http::vector_body<char>> req{
      http::verb::delete_,
      "/chunk/" + user + "/" + std::to_string(chk_id) + "/" +
          std::to_string(size) + "/" + macaron::Base64::Encode(path),
      10};
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(http::field::authorization, "Barear " + token);
  up_worker->push_request(req);
}

void RestClient::get_chunk() {}

void RestClient::delete_file(std::string &path) {
  std::shared_ptr<UpWorker> up_worker = UpWorker::getIstance();
  std::string user = config["username"];
  std::string token = config["token"];
  http::request<http::vector_body<char>> req{
      http::verb::delete_,
      "/file/" + user + "/" + macaron::Base64::Encode(path), 10};
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(http::field::authorization, "Barear " + token);
  up_worker->push_request(req);
}

void RestClient::get_status() {}

void RestClient::get_status_file() {}

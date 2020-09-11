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

void RestClient::fill_headers(http::request<http::vector_body<char>> &req,
                              size_t size) {
  req.set(http::field::host, "remote_backup_nginx-server_1");
  req.set(http::field::content_length, std::to_string(size));
  req.set(http::field::authorization, "Barear " + std::string{config["token"]});
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
}

void RestClient::post_chunk(std::tuple<std::shared_ptr<char[]>, size_t> &chunk,
                            json &jentry) {
  std::shared_ptr<UpWorker> up_worker = UpWorker::getIstance();
  std::shared_ptr<Broker> broker = Broker::getInstance();
  std::shared_ptr<char[]> buffer = std::get<0>(chunk);
  http::request<http::vector_body<char>> req{
      http::verb::post,
      "/chunk/" + std::string{config["username"]} + "/" +
          std::to_string(jentry["chunks"][0]["id"].get<int>()) + "/" +
          std::to_string(std::get<1>(chunk)) + "/" +
          std::string{jentry["chunks"][0]["hash"]} + "/" +
          std::to_string(jentry["nchunks"].get<int>()) + "/" +
          macaron::Base64::Encode(std::string{jentry["path"]}.substr(7)) + "/" +
          std::to_string(jentry["last_mod"].get<int>()),
      10};
  fill_headers(req, std::get<1>(chunk));
  std::move(buffer.get(), buffer.get() + std::get<1>(chunk),
            std::back_inserter(req.body()));
  up_worker->push_request(std::tuple(req, Message{TOPIC::ADD_CHUNK, jentry}));
}

void RestClient::put_chunk(std::tuple<std::shared_ptr<char[]>, size_t> &chunk,
                           json &jentry) {
  std::shared_ptr<UpWorker> up_worker = UpWorker::getIstance();
  std::shared_ptr<Broker> broker = Broker::getInstance();
  std::shared_ptr<char[]> buffer = std::get<0>(chunk);
  size_t size = std::get<1>(chunk);
  http::request<http::vector_body<char>> req{
      http::verb::put,
      "/chunk/" + std::string{config["username"]} + "/" +
          std::to_string(jentry["chunks"][0]["id"].get<int>()) + "/" +
          std::to_string(size) + "/" +
          std::string{jentry["chunks"][0]["hash"]} + "/" +
          macaron::Base64::Encode(std::string{jentry["path"]}.substr(7)) + "/" +
          std::to_string(jentry["last_mod"].get<int>()),
      10};
  fill_headers(req, size);
  std::move(buffer.get(), buffer.get() + size, std::back_inserter(req.body()));
  up_worker->push_request(
      std::tuple(req, Message{TOPIC::UPDATE_CHUNK, jentry}));
}

void RestClient::delete_chunk(json &chk_info, size_t size) {
  std::shared_ptr<UpWorker> up_worker = UpWorker::getIstance();
  std::shared_ptr<Broker> broker = Broker::getInstance();
  http::request<http::vector_body<char>> req{
      http::verb::delete_,
      "/chunk/" + std::string{config["username"]} + "/" +
          std::to_string(chk_info["chunks"][0]["id"].get<int>()) + "/" +
          macaron::Base64::Encode(std::string{chk_info["path"]}.substr(7)),
      10};
  fill_headers(req);
  up_worker->push_request(
      std::tuple(req, Message{TOPIC::DELETE_CHUNK, chk_info}));
}

void RestClient::get_chunk() {}

void RestClient::delete_file(std::string &path) {
  std::shared_ptr<UpWorker> up_worker = UpWorker::getIstance();
  std::shared_ptr<Broker> broker = Broker::getInstance();
  json jentry;
  jentry["path"] = path;
  http::request<http::vector_body<char>> req{
      http::verb::delete_,
      "/file/" + std::string{config["username"]} + "/" +
          macaron::Base64::Encode(path.substr(7)),
      10};
  fill_headers(req);
  up_worker->push_request(
      std::tuple(req, Message{TOPIC::REMOVE_ENTRY, jentry}));
}

void RestClient::get_status() {}

void RestClient::get_status_file() {}

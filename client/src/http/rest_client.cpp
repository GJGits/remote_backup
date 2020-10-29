#include "../../include/http/rest_client.hpp"

RestClient::RestClient() {
  config = json::object();
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
  http_client = HTTPClient::getInstance();
}

void RestClient::read_info() {
  if (!std::filesystem::exists("./config/client-conf.json"))
    throw FileConfigNotValid();
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
  DurationLogger log{"POST_CHUNK"};
  std::shared_ptr<char[]> buffer;
  size_t size = 0;

  {
    DurationLogger log{"COPY_PARAMS"};
    buffer = std::get<0>(chunk);
    size = std::get<1>(chunk);
  }

  http::request<http::vector_body<char>> req{post_prototype};
  {
    DurationLogger log{"SET_HEADERS"};
    req.target(
        "/chunk/" +
        std::to_string(jentry["transfers"]["chunks"][0]["id"].get<int>()) +
        "/" + std::string{jentry["transfers"]["chunks"][0]["hash"]} + "/" +
        std::to_string(jentry["transfers"]["nchunks"].get<int>()) + "/" +
        macaron::Base64::Encode(std::string{jentry["path"]}) + "/" +
        std::to_string(jentry["last_local_change"].get<int>()));
    req.set(http::field::content_length, std::to_string(size));
    req.set(http::field::authorization,
            "Bearer " + std::string{config["token"]});
  }

  {
    DurationLogger log{"SET_BODY"};
    for (size_t i = 0; i < size; i++)
      req.body().push_back(buffer.get()[i]);
  }

  http_client->up_request(req);
}

std::vector<char> RestClient::get_chunk(const json &chunk_info) {
  http::request<http::vector_body<char>> req{get_prototype};
  // chunk + id + file_path_base64
  req.target("/chunk/" + std::to_string(chunk_info["id"].get<int>()) + "/" +
             std::string{chunk_info["path"]});
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  return http_client->get_binary(req);
}

void RestClient::delete_file(std::string &path) {
  json jentry;
  jentry["path"] = path;
  http::request<http::vector_body<char>> req{delete_prototype};
  req.target("/file/" + macaron::Base64::Encode(path));
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  http_client->up_request(req);
}

json RestClient::get_status_list(size_t page) {
  std::shared_ptr<HTTPClient> http_client = HTTPClient::getInstance();
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  http::request<http::vector_body<char>> req{get_prototype};
  req.target("/status/list/" + std::to_string(page) + "/" +
             std::to_string(sync->get_last_check()));
  req.set(http::field::authorization, "Bearer " + std::string{config["token"]});
  return http_client->get_json(req);
}

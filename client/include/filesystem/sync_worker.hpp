#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unordered_map>
#include <vector>
#include <memory>

#include "../common/client-conf.hpp"
#include "../common/base64.hpp"
#include "../common/json.hpp"
#include "http_req.hpp"
#include "message.hpp"
#include "sync_structure.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using json = nlohmann::json;

class SyncWorker {
private:
  std::vector<std::string> loopback_filter_cache;
  std::queue<HTTPReq> down_reqs;
  std::queue<http::request<http::vector_body<char>>> up_reqs;
  std::mutex mex_m;
  std::mutex cache_m;
  std::mutex down_m;
  std::mutex up_m;
  std::condition_variable mex_cv, cache_cv, down_cv, up_cv;
  std::thread sync_worker;
  std::vector<std::thread> down_workers, up_workers;
  bool finish;
  inline static SyncWorker *instance = nullptr;

  SyncWorker() {}

public:
  ~SyncWorker() {
    finish = true;
    sync_worker.join();
    for (size_t i = 0; i < down_workers.size(); i++) {
      down_workers[i].join();
    }
    for (size_t i = 0; i < up_workers.size(); i++) {
      up_workers[i].join();
    }
  }

  static SyncWorker *getInstance() {
    if (instance == nullptr) {
      instance = new SyncWorker{};
    }
    return instance;
  }

  void run_sync_worker() {
    finish = false;
    sync_worker = std::move(std::thread{[&]() {
      HandleWatcher *dispatcher = HandleWatcher::getInstance();
      while (!finish) {

        std::shared_ptr<Message> raw_message = dispatcher->pop_message(1);

        if (raw_message->getType() != -1) {

          std::shared_ptr<SyncMessage> message =
              std::dynamic_pointer_cast<SyncMessage>(raw_message);

          switch (message->getCode()) {
          case MESSAGE_CODE::UPLOAD:
            upload(message->getArgument()[0]);
          case MESSAGE_CODE::BULK_UPLOAD:
            bulk_upload(message->getArgument());
            break;
          case MESSAGE_CODE::BULK_DELETE:
            bulk_delete(message->getArgument());
            break;
          case MESSAGE_CODE::REMOVE:
            remove(message->getArgument()[0]);
            break;
          case MESSAGE_CODE::UPDATE_PATH:
            rename(message->getArgument()[0], message->getArgument()[1]);
            break;

          default:
            break;
          }
        }
      }
    }});
  }

  void run_down_workers() {
    down_workers.emplace_back([&]() {
      while (!finish) {
        std::unique_lock lk{down_m};
        if (!down_reqs.empty()) {
          // job here
        }
        down_cv.wait(lk, [&]() { return !down_reqs.empty() || finish; });
      }
    });
  }

  void run_up_workers() {

    up_workers.emplace_back([&]() {
      const char *host = "remote_backup_nginx-server_1";
      const char *port = "80";
      net::io_context ioc;
      tcp::resolver resolver(ioc);
      beast::tcp_stream stream(ioc);
      auto const results = resolver.resolve(host, port);
      while (!finish) {
        std::unique_lock lk{up_m};
        if (!up_reqs.empty()) {
          http::request<http::vector_body<char>> req =
              up_reqs.front();
          up_reqs.pop();
          req.set(http::field::host, host);
          req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
          http::write(stream, req);
        }
        up_cv.wait(lk, [&]() { return !up_reqs.empty() || finish; });
      }
    });
  }

  void
  push_up_req(const http::request<http::vector_body<char>> &req) {
    std::unique_lock lk{up_m};
    up_reqs.push(req);
    up_cv.notify_all();
  }

  void bulk_upload(const std::vector<std::string> &paths) {
    for (std::string path : paths)
      upload(path);
  }

  void upload(const std::string &path) {
    std::clog << "ENTRO IN UPLOAD\n";
    HandleWatcher *dispatcher = HandleWatcher::getInstance();
    std::shared_ptr<ClientConf> conf = ClientConf::getInstance();
    json entry;
    int fsize = (int)std::filesystem::file_size(path);
    int seek_pos = 0;
    std::ifstream file(path, std::ios::binary);
    struct stat fileInfo;
    stat(path.c_str(), &fileInfo);
    entry["path"] = path;
    entry["size"] = fsize;
    entry["validity"] = false;
    entry["dim_last_chunk"] =
        fileInfo.st_size > 0 ? (int)(fileInfo.st_size % CHUNK_SIZE) : -1;
    entry["last_mod"] = fileInfo.st_mtim.tv_sec;
    std::unique_ptr<char[]> read_buf(new char[CHUNK_SIZE + 1]);
    int i = 0;
    while (seek_pos < fsize) {
      memset(read_buf.get(), '\0', CHUNK_SIZE);
      file.seekg(seek_pos);
      size_t to_read =
          (fsize - seek_pos) >= CHUNK_SIZE ? CHUNK_SIZE : (fsize - seek_pos);
      file.read(read_buf.get(), to_read); // todo: check su read
      std::vector<char> chunk_buf{read_buf.get(), read_buf.get() + to_read};
      std::string hash_chunk = Sha256::getSha256(chunk_buf);

      http::request<http::vector_body<char>> req{
          http::verb::post,
          "/chunk/" + conf->getUser() + "/" + std::to_string(i) + "/" +
              std::to_string(to_read) + "/" + macaron::Base64::Encode(path),
          10};

      req.set(http::field::content_length, chunk_buf.size());
      std::move(chunk_buf.begin(), chunk_buf.end(), std::back_inserter(req.body()));
      // up_reqs.push(req); todo: decommentare quando server ok

      json chunk;
      chunk["id"] = i;
      chunk["hash"] = hash_chunk;
      entry["chunks"].push_back(chunk);
      dispatcher->push_message(std::make_shared<StructMessage>(
          StructMessage{MESSAGE_CODE::ADD_CHUNK, entry}));
      seek_pos += to_read;
      i++;
      entry["chunks"].clear();
    }
  }

  void bulk_delete(const std::vector<std::string> &paths) {
    std::shared_ptr<ClientConf> conf = ClientConf::getInstance();
    std::string base_64_paths;
    for (size_t i = 0; i < paths.size() - 1; i++) {
      base_64_paths += macaron::Base64::Encode(paths[i]) + "-";
    }
    base_64_paths += macaron::Base64::Encode(paths[paths.size() - 1]);
    http::request<http::vector_body<char>> req{
        http::verb::delete_, "/file/" + conf->getUser() + "/" + base_64_paths, 10};
    // up_reqs.push(std::move(req)); todo: decommentare quando server ok
  }

  void remove(const std::string &path) {
    // todo: eliminare da server
  }

  void rename(const std::string &old_path, const std::string &new_path) {}
};
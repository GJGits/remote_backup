#pragma once

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <thread> // per debugging su thread
#include "../filesystem/linux_watcher.hpp"
#include "../pubsub/sync_sub.hpp"
#include "../modules/module.hpp"

using boost::asio::ip::udp;

class GuiModule : public Singleton<GuiModule>, public ModuleManager {

private:
  friend class Singleton;
  bool running;
  boost::asio::io_context io_context{};
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 1024> recv_buffer_;

  GuiModule();

public:
  ~GuiModule();
  void init_sub_list();
  void on_transfer(const Message &message);
  void on_easy_exception(const Message &message);
  void on_auth_failed(const Message &message);
  void on_connection_lost(const Message &message);
  void add_module(const std::shared_ptr<Module> &module);
  void start();
  void close();
  void start_receive();
  void handle_gui_message();
  void send_message(const json &message);
};
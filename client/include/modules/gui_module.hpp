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
  void register_signal();
  void on_transfer(const Message &message);
  void on_init_remote_sync();
  void on_finish_remote_sync();
  void on_easy_exception();
  void on_auth_failed();
  void on_connection_lost();
  void add_module(const std::string &name, const std::shared_ptr<Module> &module);
  void start();
  void close();
  void start_receive();
  void handle_gui_message();
  void send_message(const json &message);
};

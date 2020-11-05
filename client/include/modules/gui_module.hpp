#pragma once

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread> // per debugging su thread
#include "../modules/module.hpp"
#include "../common/singleton.hpp"

using boost::asio::ip::udp;

class GuiModule : public Singleton<GuiModule>, public ModuleManager {

private:
  friend class Singleton;
  bool running;
  boost::asio::io_context io_context{};
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 1024> recv_buffer_;

  GuiModule()
      : running{false}, socket_(io_context, udp::endpoint(udp::v4(), 2800)) {
    std::clog << "gui module init...\n";
  }

public:
  ~GuiModule() { std::clog << "gui module destroyx...\n"; }
  void init_sub_list();
  void add_module(const std::shared_ptr<Module> &module);
  void start();
  void close();
  void start_receive();
  void handle_gui_message();
  void send_message(const Message &message);
};
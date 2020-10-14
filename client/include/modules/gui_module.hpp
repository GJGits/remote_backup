#pragma once

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "../pubsub/broker.hpp"

using boost::asio::ip::udp;

class GuiModule {

private:
  bool running;
  boost::asio::io_context io_context{};
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 1024> recv_buffer_;
  static inline std::shared_ptr<GuiModule> instance{nullptr};

  GuiModule()
      : running{false}, socket_(io_context, udp::endpoint(udp::v4(), 2800)) {}

public:
  static std::shared_ptr<GuiModule> getInstance();
  void init_sub_list();
  void start();
  void start_receive();
  /**
   * recap protocollo:
   * recv_buffer[0]: topic code su 1 byte
   * recv_buffer[1],recv_buffer[2]: message length su 2 byte, ma limite a 1024
   * recv_buffer[1],... : message content in formato json che verra' parsato
   */
  void handle_gui_message();
  void send_message(const Message &message);
};
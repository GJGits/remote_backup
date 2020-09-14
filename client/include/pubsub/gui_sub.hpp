#pragma once

#include "broker.hpp"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>

using boost::asio::ip::udp;

class GuiSubscriber {
private:
  bool is_running;
  static inline std::shared_ptr<GuiSubscriber> instance{nullptr};
  GuiSubscriber() {}

public:
  static std::shared_ptr<GuiSubscriber> getInstance();
  void init();
  void on_action(const Message &message);
  void on_finish(const Message &message);
};
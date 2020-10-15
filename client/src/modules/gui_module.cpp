#include "../../include/modules/gui_module.hpp"

void GuiModule::init_sub_list() {
  std::shared_ptr<Broker> broker = Broker::getInstance();
  broker->subscribe(
      TOPIC::CLOSE,
      std::bind(&GuiModule::stop, instance.get(), std::placeholders::_1));
}

void GuiModule::start() {
  std::clog << "gui module started...\n";
  start_receive();
  running = true;
  io_context.run();
}

void GuiModule::stop(const Message &message) { 
  std::clog << "gui module closed...\n";
  running = false; 
  socket_.cancel();
  socket_.close();
  io_context.stop();
  }

void GuiModule::start_receive() {
  socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
                             remote_endpoint_,
                             std::bind(&GuiModule::handle_gui_message, this));
}

void GuiModule::handle_gui_message() {
  std::shared_ptr<Broker> broker = Broker::getInstance();
  std::clog << "ricevo da gui: " << std::to_string((int)recv_buffer_[0]) << "\n";
  broker->publish(Message{(TOPIC)recv_buffer_[0]});
  std::clog << "attendo nuovo comando\n";
  start_receive();
}

void GuiModule::send_message(const Message &message) {
  boost::asio::io_service io_service;
  udp::socket sock(io_service, udp::endpoint(udp::v4(), 0));
  udp::resolver resolver(io_service);
  udp::resolver::query query(udp::v4(), "172.17.0.1", "41234");
  udp::resolver::iterator iter = resolver.resolve(query);
  udp::endpoint endpoint = *iter;
  std::string msg{message.get_content().dump()};
  sock.send_to(boost::asio::buffer(msg, msg.size()), endpoint);
  sock.close();
}
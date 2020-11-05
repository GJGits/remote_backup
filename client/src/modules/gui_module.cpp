#include "../../include/modules/gui_module.hpp"

void GuiModule::init_sub_list() {
}

void GuiModule::add_module(const std::shared_ptr<Module> &module) {
  modules.push_back(module);
}

void GuiModule::start() {
  std::clog << "gui module started...\n";
  start_receive();
  running = true;
  io_context.run();
}

void GuiModule::close() {
  std::clog << "gui module closed...\n";
  running = false;
  socket_.cancel();
  socket_.close();
  io_context.stop();
  for (size_t i = 0; i < modules.size(); i++)
    modules[i]->stop();
}

void GuiModule::start_receive() {
  socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
                             remote_endpoint_,
                             std::bind(&GuiModule::handle_gui_message, this));
}

void GuiModule::handle_gui_message() {
  int message = (int)recv_buffer_[0];
  switch (message) {
  case 0: // message stop da gui
    for (size_t i = 0; i < modules.size(); i++)
      modules[i]->stop();
    break;
  case 1: // message start da gui
    for (size_t i = 0; i < modules.size(); i++)
      modules[i]->start();
    break;
  case 2: // message restart da gui
    for (size_t i = 0; i < modules.size(); i++)
      modules[i]->restart();
    break;
  case 3: // message exit da gui
    close();

  default:
    break;
  }
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
  std::string msg{message.get_content()->to_json().dump()};
  sock.send_to(boost::asio::buffer(msg, msg.size()), endpoint);
  sock.close();
}
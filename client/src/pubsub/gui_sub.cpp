#include "../../include/pubsub/gui_sub.hpp"

std::shared_ptr<GuiSubscriber> GuiSubscriber::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<GuiSubscriber>(new GuiSubscriber{});
  }
  return instance;
}

void GuiSubscriber::init() {
  std::shared_ptr<Broker> broker = Broker::getInstance();
  broker->subscribe(TOPIC::ADD_CHUNK,
                    std::bind(&GuiSubscriber::on_action, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::UPDATE_CHUNK,
                    std::bind(&GuiSubscriber::on_action, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::DELETE_CHUNK,
                    std::bind(&GuiSubscriber::on_action, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::FILE_DELETED,
                    std::bind(&GuiSubscriber::on_action, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::UP_EMPTY,
                    std::bind(&GuiSubscriber::on_finish, instance.get(),
                              std::placeholders::_1));
}

void GuiSubscriber::on_action(const Message &message) {
  if (!is_running) {
    boost::asio::io_service io_service;
    udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));
    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), "172.17.0.1", "41234");
    udp::resolver::iterator iter = resolver.resolve(query);
    udp::endpoint endpoint = *iter;
    std::string msg{"start-sync"};
    socket.send_to(boost::asio::buffer(msg, msg.size()), endpoint);
    socket.close();
    is_running = true;
  }
}

void GuiSubscriber::on_finish(const Message &message) {
  boost::asio::io_service io_service;
  udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));
  udp::resolver resolver(io_service);
  udp::resolver::query query(udp::v4(), "172.17.0.1", "41234");
  udp::resolver::iterator iter = resolver.resolve(query);
  udp::endpoint endpoint = *iter;
  std::string msg{"end-sync"};
  socket.send_to(boost::asio::buffer(msg, msg.size()), endpoint);
  socket.close();
  is_running = false;
}
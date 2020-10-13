#include "../../include/pubsub/gui_sub.hpp"

class udp_server {
public:
  udp_server(boost::asio::io_context &io_context)
      : socket_(io_context, udp::endpoint(udp::v4(), 2800)) {
    start_receive();
  }

private:
  void start_receive() {
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        std::bind(&udp_server::handle_receive, this));
  }

  void handle_receive() {

      std::clog << "Code received by client: " << recv_buffer_[0] << "\n";

      start_receive();
  }

  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 1> recv_buffer_;
};

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
  try {
    boost::asio::io_context io_context;
    udp_server server(io_context);
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
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
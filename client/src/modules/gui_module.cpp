#include "../../include/modules/gui_module.hpp"

void GuiModule::init_sub_list() {
  broker->subscribe(TOPIC::REMOVE_ENTRY,
                    std::bind(&GuiModule::on_transfer, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::ADD_ENTRY,
                    std::bind(&GuiModule::on_transfer, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::TRANSFER_COMPLETE,
                    std::bind(&GuiModule::on_transfer, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::EASY_EXCEPTION,
                    std::bind(&GuiModule::on_easy_exception, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::AUTH_FAILED,
                    std::bind(&GuiModule::on_auth_failed, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::CONNECTION_LOST,
                    std::bind(&GuiModule::on_connection_lost, instance.get(),
                              std::placeholders::_1));
}

void GuiModule::add_module(const std::shared_ptr<Module> &module) {
  modules.push_back(module);
}

void GuiModule::start() {
  running = true;
  std::clog << "gui module started...\n";
  start_receive();
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
  std::clog << "handle gui message: " << message << " [thread "
            << std::this_thread::get_id() << "]"
            << "\n";
  try {
    resource_guard guard{};
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
      break;

    default:
      break;
    }
    if (running) {
      std::clog << "attendo nuovo comando\n";
      start_receive();
    }
  } catch (AuthFailed &ex) {
    std::clog << ex.what() << "\n";
    on_auth_failed(Message{TOPIC::AUTH_FAILED});
  }

  catch (ConnectionNotAvaible &ex) {
    std::clog << ex.what() << "\n";
    on_connection_lost(Message{TOPIC::CONNECTION_LOST});
  }

  catch (std::ifstream::failure &ex) {
    std::clog << ex.what() << "\n";
    on_easy_exception(Message{TOPIC::EASY_EXCEPTION});
  }

  catch (std::filesystem::filesystem_error &ex) {
    std::clog << ex.what() << "\n";
    on_easy_exception(Message{TOPIC::EASY_EXCEPTION});
  }

  catch (...) {
    std::clog << "The impossible happened!\n";
    on_easy_exception(Message{TOPIC::EASY_EXCEPTION});
  }
}

void GuiModule::on_easy_exception(const Message &message) {
  for (size_t i = 0; i < modules.size(); i++)
    modules[i]->restart();
}

void GuiModule::on_auth_failed(const Message &message) {
  for (size_t i = 0; i < modules.size(); i++)
    modules[i]->stop();
  json msg = {
      {"code", "auth-failed"},
      {"message", "autenticazione fallita o scaduta, procedere con il login"}};
  send_message(msg);
}

void GuiModule::on_connection_lost(const Message &message) {
  std::clog << "connection lost handled\n";
  for (size_t i = 0; i < modules.size(); i++)
    modules[i]->stop();
  json msg = {{"code", "connection-lost"},
              {"message", "connessione persa, ricconnettersi e riprovare"}};
  send_message(msg);
}

void GuiModule::on_transfer(const Message &message) {
  json transfer = message.get_content()->to_json();
  json mex = {{"code", "transfer"}, {"message", transfer}};
  send_message(mex);
}

void GuiModule::send_message(const json &message) {
  std::clog << "invio messaggio\n";
  boost::asio::io_service io_service;
  udp::socket sock(io_service, udp::endpoint(udp::v4(), 0));
  udp::resolver resolver(io_service);
  // indirizzo ipv4 broadcast rete docker
  udp::resolver::query query(udp::v4(), "172.17.0.1", "41234");
  udp::resolver::iterator iter = resolver.resolve(query);
  udp::endpoint endpoint = *iter;
  std::string msg{message.dump()};
  sock.send_to(boost::asio::buffer(msg, msg.size()), endpoint);
  sock.close();
}
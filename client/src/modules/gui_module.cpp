#include "../../include/modules/gui_module.hpp"

GuiModule::GuiModule()
    : running{false}, socket_(io_context, udp::endpoint(udp::v4(), 2800)) {
  std::clog << "gui module init...\n";
}

GuiModule::~GuiModule() {
  if (running) {
    for (const auto &[name, module] : modules)
      module->stop();
  }

  std::clog << "gui module destroy...\n";
}

void GuiModule::init_sub_list() {
  broker = Broker::getInstance();
  broker->subscribe(TOPIC::NEW_FILE, PRIORITY::HIGH,
                    std::bind(&GuiModule::on_transfer, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::FILE_DELETED, PRIORITY::HIGH,
                    std::bind(&GuiModule::on_transfer, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::INIT_SERVER_SYNC, PRIORITY::HIGH,
                    std::bind(&GuiModule::on_init_remote_sync, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::FINISH_SERVER_SYNC, PRIORITY::HIGH,
                    std::bind(&GuiModule::on_finish_remote_sync, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::REMOVE_ENTRY, PRIORITY::HIGH,
                    std::bind(&GuiModule::on_transfer, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::ADD_ENTRY, PRIORITY::HIGH,
                    std::bind(&GuiModule::on_transfer, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::EASY_EXCEPTION, PRIORITY::HIGH,
                    std::bind(&GuiModule::on_easy_exception, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::AUTH_FAILED, PRIORITY::HIGH,
                    std::bind(&GuiModule::on_auth_failed, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::CONNECTION_LOST, PRIORITY::HIGH,
                    std::bind(&GuiModule::on_connection_lost, instance.get(),
                              std::placeholders::_1));
}

void GuiModule::add_module(const std::string &name,
                           const std::shared_ptr<Module> &module) {
  modules[name] = module;
}

void GuiModule::start() {
  running = true;
  std::clog << "gui module started...\n";
  start_receive();
  io_context.run();
}

void GuiModule::close() {
  running = false;
  socket_.cancel();
  socket_.close();
  io_context.stop();
  for (const auto &[name, module] : modules)
    module->stop();
  std::clog << "gui module closed...\n";
}

void GuiModule::start_receive() {
  socket_.async_receive_from(boost::asio::buffer(recv_buffer_),
                             remote_endpoint_,
                             std::bind(&GuiModule::handle_gui_message, this));
}

void GuiModule::handle_gui_message() {
  try {

    std::unique_lock lock{mu};
    resource_guard guard{};
    int message = (int)recv_buffer_[0];

    switch (message) {
    case 0: // message stop da gui
      for (const auto &[name, module] : modules)
        module->stop();
      break;
    case 1: // message start da gui
      RestClient::getInstance()->read_info();
      for (const auto &[name, module] : modules)
        if (name.compare("watcher") != 0)
          module->start();
      break;
    case 2: // message restart da gui
      RestClient::getInstance()->read_info();
      for (const auto &[name, module] : modules)
        module->restart();
      break;
    case 3: // message exit da gui
      close();
      break;

    default:
      break;
    }

    if (running) {
      std::clog << "attendo nuovo comando\n";
      lock.unlock();
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
    std::clog << "The impossible happened! (GuiModule)\n";
    on_easy_exception(Message{TOPIC::EASY_EXCEPTION});
  }
}

void GuiModule::on_easy_exception(const Message &message) {
  std::unique_lock lk{mu};
  DurationLogger log{"EASY_EXCEPTION"};
  for (const auto &[name, module] : modules)
    module->stop();

  for (const auto &[name, module] : modules)
    if (name.compare("watcher") != 0)
      module->start();
}

void GuiModule::on_auth_failed(const Message &message) {
  std::unique_lock lk{mu};
  for (const auto &[name, module] : modules)
    module->stop();
  json msg = {
      {"code", "auth-failed"},
      {"message", "autenticazione fallita o scaduta, procedere con il login"}};
  send_message(msg);
}

void GuiModule::on_connection_lost(const Message &message) {
  std::unique_lock lk{mu};
  DurationLogger log{"CONNECTION_LOST"};
  for (const auto &[name, module] : modules)
    module->stop();
  json msg = {{"code", "connection-lost"},
              {"message", "connessione persa, ricconnettersi e riprovare"}};
  send_message(msg);
}

void GuiModule::on_transfer(const Message &message) {
  std::unique_lock lk{mu};
  std::shared_ptr<FileEntry> entry = message.get_content();
  json transfer = message.get_content()->to_json();
  json mex = {{"code", "transfer"}, {"message", transfer}};
  send_message(mex);
}

void GuiModule::on_init_remote_sync(const Message &message) {
  std::unique_lock lk{mu};
  DurationLogger log{"START_REMOTE_SYNC"};
  modules["watcher"]->stop();
}

void GuiModule::on_finish_remote_sync(const Message &message) {
  std::unique_lock lk{mu};
  DurationLogger log{"FINISH_REMOTE_SYNC"};
  modules["watcher"]->start();
}

void GuiModule::send_message(const json &message) {
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

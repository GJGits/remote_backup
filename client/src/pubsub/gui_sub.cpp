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

void GuiSubscriber::on_action(const Message &message) {}

void GuiSubscriber::on_action(const Message &message) {}
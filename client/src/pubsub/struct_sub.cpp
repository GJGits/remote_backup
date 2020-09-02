#include "../../include/pubsub/struct_sub.hpp"

std::shared_ptr<StructSubscriber> StructSubscriber::getInstance() {
  if (instance.get() == nullptr) {
    instance = std::shared_ptr<StructSubscriber>(new StructSubscriber{});
  }
  return instance;
}

void StructSubscriber::init() {
  std::shared_ptr<Broker> broker = Broker::getInstance();
  broker->subscribe(TOPIC::ADD_CHUNK,
                    std::bind(&StructSubscriber::on_add_chunk, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::UPDATE_CHUNK,
                    std::bind(&StructSubscriber::on_update_chunk,
                              instance.get(), std::placeholders::_1));
  broker->subscribe(TOPIC::DELETE_CHUNK,
                    std::bind(&StructSubscriber::on_delete_chunk,
                              instance.get(), std::placeholders::_1));
  broker->subscribe(TOPIC::FILE_DELETED,
                    std::bind(&StructSubscriber::on_delete_entry,
                              instance.get(), std::placeholders::_1));
  broker->subscribe(TOPIC::TIME_OUT,
                    std::bind(&StructSubscriber::on_timeout,
                              instance.get(), std::placeholders::_1));
}

void StructSubscriber::on_add_chunk(const Message &message) {
  std::clog << "ADD CHUNK\n";
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->add_chunk(message.get_content());
}

void StructSubscriber::on_update_chunk(const Message &message) {
  std::clog << "UPDATE CHUNK\n";
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->replace_chunk(message.get_content());
}

void StructSubscriber::on_delete_chunk(const Message &message) {
  std::clog << "DELETE CHUNK\n";
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->delete_chunk(message.get_content());
}

void StructSubscriber::on_delete_entry(const Message &message) {
  std::clog << "DELETE ENTRY\n";
  json content = message.get_content();
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->delete_entry(message.get_content());
}

void StructSubscriber::on_timeout(const Message &message) {
  std::clog << "TIMEOUT\n";
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->write_structure();
}

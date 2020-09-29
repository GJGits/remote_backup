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
  broker->subscribe(TOPIC::FILE_MODIFIED,
                    std::bind(&StructSubscriber::on_file_modified, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::REMOVE_ENTRY,
                    std::bind(&StructSubscriber::on_delete_entry,
                              instance.get(), std::placeholders::_1));
}

void StructSubscriber::on_add_chunk(const Message &message) {
  // Se scrivessi contamporaneamente due chunk potrebbero occupare
  // la medesima posizione, di conseguenza perderei il primo.
  // L'ordine tuttavia non importa perche' ogni chunk porta con se
  // il suo id.
  std::unique_lock lk{m};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->add_chunk(message.get_content());
  sync->write_structure();
}

void StructSubscriber::on_file_modified(const Message &message) {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  json content = message.get_content();
  sync->reset_chunks(content["path"]);
}

void StructSubscriber::on_delete_entry(const Message &message) {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->delete_entry(message.get_content());
}

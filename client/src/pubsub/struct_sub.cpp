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
  broker->subscribe(TOPIC::UP_EMPTY,
                    std::bind(&StructSubscriber::on_up_empty, instance.get(),
                              std::placeholders::_1));
}

void StructSubscriber::on_add_chunk(const Message &message) {
  // Se scrivessi contamporaneamente due chunk potrebbero occupare
  // la medesima posizione, di conseguenza perderei il primo.
  // L'ordine tuttavia non importa perche' ogni chunk porta con se
  // il suo id.
  std::unique_lock lk{m};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->add_chunk(message.get_content());
}

void StructSubscriber::on_update_chunk(const Message &message) {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->replace_chunk(message.get_content());
}

void StructSubscriber::on_delete_chunk(const Message &message) {
  // Se provassi ad eliminare due chunk contemporaneamente potrei
  // tentare di accedere, scorrendo la lista dei chunk, ad uno
  // precedentemente eliminato. Anche in questo caso non importa
  // quale viene eliminato per prima.
  std::unique_lock lk{m};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->delete_chunk(message.get_content());
}

void StructSubscriber::on_delete_entry(const Message &message) {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->delete_entry(message.get_content());
}

void StructSubscriber::on_up_empty(const Message &message) {
  std::unique_lock lk{m};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->write_structure();
}

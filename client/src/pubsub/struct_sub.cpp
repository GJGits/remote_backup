#include "../../include/pubsub/struct_sub.hpp"

void StructSubscriber::init_sub_list() {
  std::shared_ptr<Broker> broker = Broker::getInstance();
  broker->subscribe(TOPIC::ADD_CHUNK,
                    std::bind(&StructSubscriber::on_add_chunk, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::REMOVE_ENTRY,
                    std::bind(&StructSubscriber::on_delete_entry,
                              instance.get(), std::placeholders::_1));
}

void StructSubscriber::start(const Message &message) {
  std::unique_lock lk{m1};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->read_structure();
}

void StructSubscriber::stop(const Message &message) {
  std::unique_lock lk{m1};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->write_structure();
}

void StructSubscriber::on_add_chunk(const Message &message) {
  // Se scrivessi contamporaneamente due chunk potrebbero occupare
  // la medesima posizione, di conseguenza perderei il primo.
  // L'ordine tuttavia non importa perche' ogni chunk porta con se
  // il suo id.
  std::unique_lock lk{m1};
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->add_chunk(message.get_content());
}

void StructSubscriber::on_delete_entry(const Message &message) {
  std::shared_ptr<SyncStructure> sync = SyncStructure::getInstance();
  sync->delete_entry(message.get_content());
}

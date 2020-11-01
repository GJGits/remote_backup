#include "../../include/pubsub/struct_sub.hpp"

void StructSubscriber::init_sub_list() {
  broker->subscribe(TOPIC::ADD_ENTRY,
                    std::bind(&StructSubscriber::on_add_entry, instance.get(),
                              std::placeholders::_1));
  broker->subscribe(TOPIC::REMOVE_ENTRY,
                    std::bind(&StructSubscriber::on_delete_entry,
                              instance.get(), std::placeholders::_1));
}

void StructSubscriber::start(const Message &message) {
  sync->restore();
  sync->update_from_fs();
  //sync->update_from_remote();
  sync->notify();
}

void StructSubscriber::stop(const Message &message) {
  sync->store();
}

void StructSubscriber::on_add_entry(const Message &message) {
  std::shared_ptr<FileEntry> entry = message.get_content();
  sync->add_entry(entry);
}

void StructSubscriber::on_delete_entry(const Message &message) {
  sync->remove_entry(message.get_content());
}

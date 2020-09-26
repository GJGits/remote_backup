#pragma once

#include <string>

class Subject {
private:
  std::string sub;
  size_t db_id;
  size_t device_id;

public:
  Subject(){}
  Subject(const std::string &sub, size_t db_id, size_t device_id)
      : sub{std::move(sub)}, db_id{db_id}, device_id{device_id} {}
  Subject(const Subject &subject)
      : sub{subject.get_sub()}, db_id{subject.get_db_id()} {}
  std::string get_sub() const { return sub; }
  size_t get_db_id() const { return db_id; }
  size_t get_device_id() const {return device_id;}
};
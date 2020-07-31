#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "base.hpp"

class File : public Base
{

private:
    uintmax_t size;
    File(std::string name, uintmax_t size) : Base(name), size{size} {}

public:
    const uintmax_t getSize();
    const virtual void ls(int indent);
    const virtual int mType();
    static std::shared_ptr<File> makeFile(std::string name, uintmax_t size);
};
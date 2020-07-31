#pragma once

#include <memory>

class Base
{
private:
    std::string name;

protected:
    Base(std::string name) : name{name} {}

public:
    const std::string getName() { return this->name; }
    const virtual int mType() = 0;
    const virtual void ls(int indent) = 0;
};
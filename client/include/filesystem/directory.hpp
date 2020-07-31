#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>

#include "base.hpp"
#include "file.hpp"

class Directory : public Base
{
private:
    std::weak_ptr<Directory> parent;
    std::weak_ptr<Directory> self;
    std::unordered_map<std::string, std::shared_ptr<Base>> children;
    Directory(std::string name) : Base(name), parent{std::weak_ptr<Directory>{}}, self{std::weak_ptr<Directory>{}} {}

public:
    static std::shared_ptr<Directory> root;
    const virtual void ls(int indent);
    const virtual int mType();
    std::weak_ptr<Base> get(const std::string &name);
    std::weak_ptr<Directory> getDir(const std::string &name);
    std::weak_ptr<File> getFile(const std::string &name);
    std::shared_ptr<Directory> addDirectory(const std::string &name);
    std::shared_ptr<File> addFile(const std::string &nome, uintmax_t size);
    bool remove(const std::string &name);
    static std::shared_ptr<Directory> getRoot();
    static std::shared_ptr<Directory> makeDir(std::string name, std::weak_ptr<Directory> parent);
    static Directory *heap(std::string name) { return new Directory(name); } //todo: valutare se necessario
};
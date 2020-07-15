#include <iostream>
#include "unistd.h"

int main()
{
    for (int i = 0; i < 1000; i++) {
        std::clog << "hello [" << i << "] by the server!\n";
        sleep(10);
    }
    return 0;
}
/*
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

int main() {
    sleep(10);
    std::cout << "STARTING....\n";
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    bsoncxx::builder::stream::document document{};

    auto collection = conn["testdb"]["testcollection"];
    document << "hello" << "world";

    collection.insert_one(document.view());
    auto cursor = collection.find({});

    for (auto&& doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }
    std::cout << "FINISH\n";
    return 0;
}
*/

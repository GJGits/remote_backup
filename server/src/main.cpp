#include "../include/test/test.hpp"
#include "unistd.h"
#include <iostream>
#include <fstream>
#include "../include/http/server.hpp"
#include "unistd.h"
#include <boost/asio.hpp>
#include <regex>

int main()
{

try {
    testAll();

    std::size_t num_threads = 4;
    http::server::server s("0.0.0.0", "3500", "/", num_threads);

    s.run();

  } catch (const std::regex_error& e) {
    std::clog << "regex_error caught: " << e.what() << '\n';
    if (e.code() == std::regex_constants::error_brack) {
        std::clog << "The code was error_brack\n";
    }
}
    catch (std::exception &e) {
    std::clog << "error on server start\n";
  }
  return 0;
}

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
    std::ofstream redirect_file("../logs/errors.log");
    // save output buffer of cout
    std::streambuf * strm_buffer = std::cout.rdbuf();
    // redirect output into file
    std::cout.rdbuf(redirect_file.rdbuf());

    testAll();

    std::size_t num_threads = 4;
    http::server::server s("0.0.0.0", "3500", "/", num_threads);

    s.run();
    std::cout.rdbuf(strm_buffer);

}
    catch (std::exception &e) {
    std::clog << "error on server start\n";
  }
  return 0;
}

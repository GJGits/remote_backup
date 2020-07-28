#include "../include/test/test.hpp"
#include "unistd.h"
#include <iostream>
#include <fstream>
#include "../include/http/server.hpp"
#include "unistd.h"
#include <boost/asio.hpp>


int main()
{
try {
    testAll();
    // Initialise the server.

     // Initialise the server.
    std::size_t num_threads = 4;
    http::server::server s("0.0.0.0", "3500", "/", num_threads);
    //http::server::server s("0.0.0.0", "3500", "/");

    // Run the server until stopped.
    s.run();

  } catch (std::exception &e) {
    std::clog << "error on sersdsdcsdcsdcdsver start\n";
  }
  return 0;
}

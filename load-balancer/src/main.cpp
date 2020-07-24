#include "../include/http/server.hpp"
#include "unistd.h"
#include <boost/asio.hpp>
#include <iostream>
#include "../include/test/test.hpp"

int main() {
  /*
  for (int i = 0; i < 1000; i++) {
      std::clog << "hello [" << i << "] by the load-balancer\n";
      sleep(10);
  }
  */

 testAll();
  try {

    // Initialise the server.
    http::server::server s("0.0.0.0", "3100", "/");

    // Run the server until stopped.
    s.run();

  } catch (std::exception &e) {
    std::clog << "error on server start\n";
  }
  return 0;
}

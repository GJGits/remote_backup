#include "../include/test/test.hpp"
#include "unistd.h"
#include <iostream>
#include <fstream>
#include "../include/http/server.hpp"
#include "unistd.h"
#include <boost/asio.hpp>
#include "../include/http/handle_rests_api.hpp"


void handle_get_status(){
    rests_api_get_status *get_status = rests_api_get_status::getInstance();
    while(1) {
        get_status->get_and_produce();
    }
}

int main()
{
try {
    std::thread get_status_thread(handle_get_status);

    // Initialise the server.
    http::server::server s("0.0.0.0", "3500", "/");

    // Run the server until stopped.
    s.run();

    get_status_thread.join();

  } catch (std::exception &e) {
    std::clog << "error on sersdsdcsdcsdcdsver start\n";
  }
  return 0;
}

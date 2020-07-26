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

void handle_post_signup(){
    rests_api_post_signup *post_signup = rests_api_post_signup::getInstance();
    while(1) {
        post_signup->get_and_produce();
    }
}

void handle_post_signin(){
    rests_api_post_signin *post_signin = rests_api_post_signin::getInstance();
    while(1) {
        post_signin->get_and_produce();
    }
}

int main()
{
try {
    testAll();
    std::thread get_status_thread(handle_get_status);
    std::thread post_signup_thread(handle_post_signup);
    std::thread post_signin_thread(handle_post_signin);
    // Initialise the server.
    http::server::server s("0.0.0.0", "3500", "/");

    // Run the server until stopped.
    s.run();

    get_status_thread.join();
    post_signup_thread.join();
    post_signin_thread.join();

  } catch (std::exception &e) {
    std::clog << "error on sersdsdcsdcsdcdsver start\n";
  }
  return 0;
}

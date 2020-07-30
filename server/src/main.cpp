#include "../include/test/test.hpp"
#include "unistd.h"
#include <iostream>
#include <fstream>

int main()
{
    sleep(2);
    std::ofstream redirect_file("../logs/errors.log");
    // save output buffer of cout
    std::streambuf * strm_buffer = std::cout.rdbuf();
    // redirect output into file
    std::cout.rdbuf(redirect_file.rdbuf());
    //testAll();
    for (int i = 0; i < 1000; i++) {
        sleep(10);
    }
    // restore old buffer
    std::cout.rdbuf(strm_buffer);
    return 0;
}

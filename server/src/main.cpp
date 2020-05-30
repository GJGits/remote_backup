#include <iostream>
#include "unistd.h"

int main()
{
    for (int i = 0; i < 10; i++) {
        std::clog << "hello [" << i << "] by the server!\n";
        sleep(10);
    }
    return 0;
}

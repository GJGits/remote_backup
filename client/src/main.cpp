#include <iostream>
#include "unistd.h"

int main()
{
    for (int i = 0; i < 10; i++) {
        std::clog << "hello [" << i << "] by the client!\n";
        sleep(8);
    }
    return 0;
}
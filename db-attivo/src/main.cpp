#include <iostream>
#include "unistd.h"

int main()
{
    for (int i = 0; i < 1000; i++) {
        std::clog << "hello [" << i << "] by db-attivo!\n";
        sleep(10);
    }
    return 0;
}

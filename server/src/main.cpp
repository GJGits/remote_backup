#include <iostream>
#include "unistd.h"
#include "lib/test.hpp"

int main()
{   
    sleep(5);
    testAll();
    for (int i = 0; i < 1000; i++) {
        sleep(10);
    }
    return 0;
}

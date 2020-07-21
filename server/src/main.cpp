#include "lib/test.hpp"
#include "unistd.h"
#include <iostream>

int main()
{
    //freopen("../logs/errors.log", "w", stdout);
    //freopen("../logs/errors.log", "w", stderr);
    sleep(5);
    testAll();
    for (int i = 0; i < 1000; i++) {
        sleep(10);
    }
    return 0;
}

#pragma once

#include <string.h>
#include <time.h>
#include <iostream>
#include "color.hpp"

class DurationLogger {
    private:
        char *name;
        int t0;
    public:
        DurationLogger(const char *name);
        ~DurationLogger();
};

#include "utils/storage/storage.h"
#include <iostream>
#include <math.h>

/*
 * Simple infinite rectangular pulse of 1A every 0.1 sec
 */

static double dV;
static double* t;
static const double PERIOD = 0.1;

extern "C" int configure (int argc, char** argv) {
 //std::cout << argv[1] << std::endl;
 t = (double*) (storage_get("time", nullptr, nullptr, nullptr));
 std::cout << "t = " << *t << std::endl;
 
 t = (double*) storage_get(argv[0], nullptr, nullptr, nullptr);
 std::cout << "time = " << *t << std::endl;
 storage_register(argv[1], &dV);
 return 0;
}

extern "C" int execute () {
dV = sin((*t) / PERIOD * 2.0 * M_PI)>=0.0 ? 1.0:0.0;
}

extern "C" void help () {
    std::cout << "Hello World" << std::endl;
}

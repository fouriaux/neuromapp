#include "utils/storage/storage.h"
#include <iostream>

static double* t;
static double* I;
static double* V;


extern "C" int configure (int argc, char** argv) {
    t = (double*) storage_get(argv[0], nullptr, nullptr, nullptr);
    V = (double*) storage_get(argv[1], nullptr, nullptr, nullptr);
    I = (double*) storage_get(argv[2], nullptr, nullptr, nullptr);
}

extern "C" int execute () {
    std::cout << *t << " ms :\tI = " << *I << "mA\t V = " << *V << "mV" << std::endl;
}
extern "C" void help () {
    std::cout << "Hello World" << std::endl;
}

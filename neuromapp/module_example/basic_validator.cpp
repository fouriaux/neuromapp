#include "utils/storage/storage.h"
#include <iostream>

static double* t;
static double* I;
static double* V;


int configure (int argc, char** argv) {
    t = storage_get(argv[0]);
    V = storage_get(argv[1]);
    I = storage_get(argv[2]);
}

int execute () {
    std::cout << *t << " ms :\tI = " << *I << "mA\t V = " << *V << "mV" << std::endl;
}

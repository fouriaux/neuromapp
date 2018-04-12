#include "utils/storage/storage.h"
#include <iostream>
#include <math.h>
static double* t;
static double *I;
static double tau   = 0.10;
static double Vrest = -70.0;
static double V     = Vrest;
static double R     = 1;
extern "C" int configure (int argc, char** argv){
   t     = (double*) storage_get (argv[0],   nullptr, nullptr, nullptr);
   I     = (double*) storage_get (argv[1],   nullptr, nullptr, nullptr);
   storage_register (argv[2], &V);
}

extern "C" int execute () {
  V = Vrest + R*(*I)*(1- exp(-(*t)/tau));
}
extern "C" void help () {
    std::cout << "Hello World" << std::endl;
}


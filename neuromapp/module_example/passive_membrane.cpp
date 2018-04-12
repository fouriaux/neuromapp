#include "utils/storage/storage.h"
#include <math>
static double* t;
static double *I;
static double tau   = 0.10;
static double Vrest = -70.0;
static double V     = Vrest;
static double R     = 1;
int configure (int argc, char** arg) {
   t     = (double*) storage_get (argv[0],   nullptr, nullptr);
   I     = (double*) storage_get (argv[1],   nullptr, nullptr);
   storage_register (argv[2], &V);
}

int execute () {
  V = Vrest + R*(*I)*(1- exp(-(*t)/tau));
}

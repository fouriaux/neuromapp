#include "utils/storage/storage.h"
#include <math>

/*
 * Simple infinite rectangular pulse of 1A every 0.1 sec
 */

static double dV;
static double* t;
static const double PERIOD = 0.1;

int configure (int argc, char** argv) {
 t = storage_get(argv[0]);
 storage_register(argv[1], &dV);

}

int execute () {
*dV = sin((*t) / PERIOD * 2.0 * M_PI)>=0.0 ? 1.0:0.0;
}

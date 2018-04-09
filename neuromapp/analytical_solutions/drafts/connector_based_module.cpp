#include "cable_section/options.h"


// Memory Type must have same interface as a raw pointer

static OutputConnector  V; // this is output  of our module
static OutputConnector  I; // this is output of our module
static InputConnector   g;
static InputConnector   Vrest;

int configure (int argc, char** argv) {
// retreive current and voltage pointers I and V
// setting parameters of the node: delta_t , delta_x etc...
// Those are pointers to data

 V.set([](double x) {return Vrest(x);});
 I.set([](double x) {return g(x) * (Vrest(x) - Vrest(x));}
}


/*
    solving for a Constant input
 */

int execute () {
}



static double* t;
static double* V;
static double *I;
static double* tau;


int configure (int argc, char** arg) {


}

int execute () {
  *V = (*I)*exp(-t)
}

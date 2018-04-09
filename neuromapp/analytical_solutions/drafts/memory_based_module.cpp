

// Memory Type must have same interface as a raw pointer

static OutputConnector  V; // this is output  of our module
static OutputConnector  I; // this is output of our module
static InputConnector   g;
static InputConnector   Vrest;

int configure (int argc, char** argv) {

}


/*
    solving for a Constant input
 */
int execute () {
    auto g_val    = g();
    auto rest_val = Vrest();
    for (int x = 0; x < V.size(); x++) {
        V [x] = rest_val;
        I [x] = g_val * (rest_val-rest_val);
    }
}

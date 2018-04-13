#include "utils/storage/storage.h"

#include "coreneuron_1.0/solver/hines.h"
#include "coreneuron_1.0/solver/solver.h"


#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/util/timer.h"

#include "utils/error.h"


void validate (NrnThread* nrn, double* V, int nb_comp) {
    std::cout << "Relatives errors ";
    for (int i = 0; i < nb_comp; i++) {
       std::cout << abs(nrn.V[i] - V[i])/ V[i] << "\t";
    }
    std::cout << "---" << std::endl;
}


void constant_cable (   double vrest, int nb_comp, 
                        double V[nb_comp], 
                        double I[nb_comp]) {
    for (int x = 0; x < nb_comp; x++) {
        V [x] = vrest;
        I [x] = 0.0;
    }
}



int main (int argc, char** argv) {

 NrnThread* nrn = (NrnThread *) storage_get("nrn", make_nrnthread, p.d, free_nrnthread);
    int nb_comp = nrn->end;

    double* V = (double*) malloc (size_of(double) * nb_comp);
    double* I = (double*) malloc (size_of(double) * nb_comp);
    for(int i=0; i < p.step; ++i){
        constant_cable(vrest, nb_comp, V, I, g);
        
        update_current(nrn, I);
        nrn_solve_minimal(nrn);
        
        validate(nrn, V, nb_comp);
    }
    return 0;
}

#include "coreneuron_1.0/solver/helper.h"
#include "coreneuron_1.0/solver/hines.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "utils/module/module.h"

static NrnThread* nt = NULL;

int hines_solver_configure (int argc, char**argv) {
    struct input_parameters p;
    int error = MAPP_OK; //so far, so good
    error = solver_help(argc, argv, &p);
    if(error != MAPP_OK)
        return error;
    nt = (NrnThread *) storage_get (p.name,  make_nrnthread, p.d, free_nrnthread);

    if(nt == NULL){
        storage_clear(p.name);
        return MAPP_BAD_DATA;
    }
}

int hines_solver_execute () {
    nrn_solve_minimal(nt);
}

// Module interface
Module hines_solver = { .configure = &hines_solver_configure, .execute = &hines_solver_execute };

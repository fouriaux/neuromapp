/*
 * Neuromapp - main.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * francesco.cremonesi@epfl.ch
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/solver/main.c
 * \brief Miniapp about the Hines Solver
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h>
#include <assert.h>

#include "utils/storage/storage.h"
#include "utils/error.h"

#include "coreneuron_1.0/solver/helper.h"
#include "coreneuron_1.0/solver/solver.h"
#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/util/timer.h"


typedef struct SolverPlugin {
    char    library_name [256];
    char    solver_name  [256];
    void*   library_handle;
    NrnThread* (*prepare)(NrnThread* _nt);
    NrnThread* (*restore)(NrnThread* _nt);
    void (*nrn_solve_minimal)(NrnThread* _nt);
} SolverPlugin;

static SolverPlugin current_solver = {
   .library_handle    = NULL
};

int aSolverIsLoaded () {
    return (current_solver.library_handle && current_solver.nrn_solve_minimal);
}
int clearSolver () {
    if (current_solver.library_handle)
        dlclose (current_solver.library_handle);
    strcpy  (current_solver.library_name,"undefined");
    strcpy  (current_solver.solver_name, "undefined");
    current_solver.library_handle      = NULL;
    current_solver.nrn_solve_minimal   = NULL;
    current_solver.prepare             = NULL;
    current_solver.restore             = NULL;
}

// NOTE: passing a file as NULL will get default solver loaded (Hines solver, if this one is statically linked)
int loadSolver (const char* file, const char* name) {
    if (strcmp (file, current_solver.library_name) == 0) {
        return 0;
    }
    clearSolver();
    if (! (current_solver.library_handle = dlopen(name, RTLD_NOW))) {
        printf("\n%s\n", dlerror());
        clearSolver();
        return -1;
    }
    if (!(current_solver.nrn_solve_minimal = dlsym (current_solver.library_handle, "nrn_solve_minimal"))) {
            printf("\n%s\n", dlerror());
            clearSolver();
            return -1;
    }
    if (!(current_solver.prepare = dlsym (current_solver.library_handle, "prepare"))) {
            printf("\n%s\n", dlerror());
            clearSolver();
            return -1;
    }
    if (!(current_solver.restore = dlsym (current_solver.library_handle, "restore"))) {
            printf("\n%s\n", dlerror());
            clearSolver();
            return -1;
    }
    strcpy (current_solver.library_name, file);
    strcpy (current_solver.solver_name, name);
}

int coreneuron10_solver_execute(int argc, char * const argv[])
{
    struct input_parameters p;
    int error = MAPP_OK; //so far, so good
    error = solver_help(argc, argv, &p);
    if(error != MAPP_OK)
        return error;
    if (! (loadSolver(p.plugin_lib, "Hines") && aSolverIsLoaded())) {
        return -1;
    }
    NrnThread * nt = (NrnThread *) storage_get (p.name,  make_nrnthread, p.d, free_nrnthread);

    if(nt == NULL){
        storage_clear(p.name);
        return MAPP_BAD_DATA;
    }
    if (current_solver.prepare != NULL){
        nt = current_solver.prepare (nt);
    }
    gettimeofday(&tvBegin, NULL);
    current_solver.nrn_solve_minimal(nt);
    gettimeofday(&tvEnd, NULL);
    if (current_solver.restore != NULL){
        nt = current_solver.restore (nt);
    }
    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("\n Time For %s Solver : %ld [s] %ld [us]", current_solver.solver_name, tvDiff.tv_sec, (long) tvDiff.tv_usec);

    return error;
}

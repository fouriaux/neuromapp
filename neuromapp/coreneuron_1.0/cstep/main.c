/*
 * Neuromapp - main.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Bruno Magalhaes - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * bruno.magalhaes@epfl.ch * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/cstep/main.c
 * \brief Implements a miniapp combining  the compute kernel and the Hines solver of coreneuron 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "utils/storage/storage.h"

#include "coreneuron_1.0/solver/hines.h"
#include "coreneuron_1.0/solver/solver.h"

#include "coreneuron_1.0/kernel/kernel.h"
#include "coreneuron_1.0/kernel/mechanism/mechanism.h"

#include "coreneuron_1.0/cstep/helper.h"
#include "coreneuron_1.0/cstep/cstep.h"

#include "coreneuron_1.0/common/memory/nrnthread.h"
#include "coreneuron_1.0/common/util/nrnthread_handler.h"
#include "coreneuron_1.0/common/util/timer.h"

#include "utils/error.h"


#define VEC_RHS(i) (_nt->_actual_rhs[(i)])
#define VEC_V(i) (_nt->_actual_v[(i)])

static void update (NrnThread* _nt) {
    double* vec_v = &(VEC_V(0));
    double* vec_rhs = &(VEC_RHS(0));
    for (int i = 0; i < _nt->end; ++i) {
           vec_v[i] += vec_rhs[i];
    }
}

static void implicit_step (NrnThread* nt) {
     //Load mechanisms
     mech_current_NaTs2_t(nt,&(nt->ml[17]));
     mech_current_Ih(nt,&(nt->ml[10]));
     mech_current_ProbAMPANMDA_EMS(nt,&(nt->ml[18]));

     //Call solver
     nrn_solve_minimal(nt);

     //Update the states
     mech_state_NaTs2_t(nt,&(nt->ml[17]));
     mech_state_Ih(nt,&(nt->ml[10]));
     mech_state_ProbAMPANMDA_EMS(nt,&(nt->ml[18]));
}

int coreneuron10_cstep_execute(int argc, char * const argv[]) {
    struct input_parameters p;

    int error = MAPP_OK;
    error = cstep_help(argc, argv, &p);
    if(error != MAPP_OK)
        return error;

    //duplicate the data
    NrnThread ** ntu = malloc(sizeof(NrnThread*)*p.duplicate);

    //Gets the data
    ntu[0] = (NrnThread *) storage_get(p.name, make_nrnthread, p.d, free_nrnthread);
    if(ntu[0] == NULL){
        storage_clear(p.name);
        return MAPP_BAD_DATA;
    }

    for(int i=1; i<p.duplicate; ++i)
        ntu[i] = (NrnThread *) clone_nrnthread(ntu[0]);

    //Initial mechanisms set-up already done in the input date (no need to call mech_init_Ih, etc)
    gettimeofday(&tvBegin, NULL);

    for(int i=0; i < p.step; ++i){
        for(int j=0; j < p.duplicate; ++j){
            for(int k=0; k < p.mindelay_step; k++){ //loop inside min delay
                implicit_step(ntu[j]);
            }
        }
    }

    gettimeofday(&tvEnd, NULL);
    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);

    printf("\nTime for full computational step: %ld [s] %ld [us]\n", tvDiff.tv_sec, (long) tvDiff.tv_usec);

    for(int i=1 ; i < p.duplicate; ++i)
        free_nrnthread(ntu[i]);
    free(ntu);
    return error;
}

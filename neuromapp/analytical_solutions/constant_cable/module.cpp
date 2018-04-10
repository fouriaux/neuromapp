/*
 * Neuromapp - helper.c, Copyright (c), 2018,
 * Jeremy FOURIAUX - Swiss Federal Institute of technology in Lausanne,
 * jeremy.fouriaux@epfl.ch,
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/analytical_solutions/constant_cable.cpp
 * \brief Stereotypical module implementation
 */



#include <cstdlib>
#include "utils/error.h"
#include "utils/storage/storage.h"
#include "analytical_solutions/helper.h"

// Memory Type must have same interface as a raw pointer

static double*  V;      // this is output  of our module
static double*  I;      // this is output of our module
static double*  g;      // conductance 
static double*  Vrest;  // Resting voltage 
static Parameters p;


extern "C" int constant_cable_configure (int argc, char** argv) {
    help(argc, argv, &p);
    int status = mapp::MAPP_OK;
    V       = (double*) storage_get(    p.voltage,
                                        [] (void* n) -> void* {return malloc( (*(int*)n)*sizeof(double));},
                                        &p.size,
                                        [](void* p) {free(p);});
    I       = (double*) storage_get(    p.current,
                                        [] (void* n) -> void* {return malloc((*(int*)n)*sizeof(double));},
                                        &p.size,
                                        [](void* p) {free(p);});
    g       = (double*) storage_get(    p.capacitance,
                                        [] (void* n) -> void* {return malloc((*(int*)n)*sizeof(double));},
                                        &p.size,
                                        [](void* p) {free(p);});
    Vrest   = (double*) storage_get(    p.vrest,
                                        [] (void* n) -> void* {return malloc((*(int*)n)*sizeof(double));},
                                        &p.size,
                                        [](void* p) {free(p);});
    return status;
}

extern "C" int constant_cable_execute () {
    double vrest = (*Vrest);
    for (int x = 0; x < p.size; x++) {
        V [x] = vrest;
        I [x] = g[x] * (vrest-vrest);
    }
}

extern "C" int constant_cable_help () {
    return print_usage();
}

/*
 *                           Solving HH model using Forward Euler Method
 *
 *     dv      1   /                                                        \
 *     ---  = ----*| I - g_Na*m³*h(v-E_Na) - G_K*n⁴(v - E_K) - gl(v - El)   |
 *     dt      Cm  \                                                        /
 *
 *     dn
 *     ---  =  alpha_n(v)*(1-n) - beta_n(v)*(n)
 *     dt
 *
 *     idem for m and h
 *
 * Neuromapp, Copyright (c), 2018,
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
 *
 */
#include <iostream>
#include <math.h>
#include <cstdlib>
#include "utils/error.h"

static const double PERIOD = 0.1;

static const double E_k  = -72.14;   // (mV)
static const double E_Na = 55.17;    // (mV)
static const double E_l  = -49.42;   // (mV)
static       double g_K  = 0.36;     //  ion channels conductances (mS/cm²)
static       double g_Na = 1.2;      //  ion channels conductances (mS/cm²)
static       double g_l  = 0.003;    //  ion channels conductances (mS/cm²)

static double v_euler = -60.0;       //  Potential at t computed by Forward Euler method
static double Cm      = 0.01;        //  Membrane capacitance (µF/cm²)
static double I       = 0.0;
static double n = 0.0;
static double m = 0.0;
static double h = 0.0;
static double delta_t = 0.005; // timestep size (ms) 
static const double end_of_times = 200.0;

double get_alpha_n (double v) {
    return (0.01*(v + 50.0))/(1 - exp(-(v + 50.0)/10.0));
}

double get_beta_n  (double v) {
    return 0.125*exp(-(v + 60.0)/80.0);
}

double get_alpha_m (double v) {
    return (0.1*(v+35.0))/(1- exp(-(v+35.0)/10.0));
}

double get_beta_m (double v) {
    return 4.0*exp(-0.0556*(v+60.0));
}

double get_alpha_h (double v) {
    return 0.07*exp(-0.05*(v+60.0));
}

double get_beta_h (double v) {
    return 1/(1+exp(-0.1*(v+30.0)));
}

void forward_euler (double I, double& v) {

    // solve potential
    double dv = (1/Cm) * (I - g_Na*pow(m,3)*h*(v- E_Na) - g_K*pow(n,4)*(v-E_k) - g_l*(v-E_l));
     v += dv*delta_t;
    
    // compute channel states
    double dn = get_alpha_n(v)*(1-n) - get_beta_n(v)*n;
     n += dn * delta_t;
    
    double dm = get_alpha_m(v)*(1-m) - get_beta_m(v)*m;
     m += dm * delta_t;
    
    double dh = get_alpha_h(v)*(1-h) - get_beta_h(v)*h;
     h += dh * delta_t;
}

void init (double& v) {
    v = -60.0;
    I = 0;
    for (double init_t = 0; init_t < 20.0; init_t+=delta_t) {
        forward_euler(I, v_euler);
    }
}

double zero_current (double t) {
    return 0.0;
}

double constant_current(double t) {
    return 0.1;
}

double pulse (double t) {
    double value    = sin(t/PERIOD * 2.0 * M_PI);
    return  (value >= 0.0 && value <= 0.5) ? 1.0:0.0;
}

int read_data (const char* input_file) {
 return 0;
}

void printHeader () {
    std::cout << "time(ms),method,potential(mV),n,m,h,g,I(mAmp)" << std::endl;
}

void print (double t) {
   std::cout << t << "," << "ForwardEuler," 
             << v_euler << ","
             << n << ","
             << m << ","
             << h << ","
             << m*m*m*h*g_Na + n*n*n*n*g_K << ","
             << I << std::endl;
}

int hh_fe_execute (int argc, char** argv) {
    double t = 0.0;
    if(argc == 2) {
        delta_t = atof(argv[1]);
    } else {
        std::cout << "Usage: hh_fe Δt" << std::endl;
        std::cout << "Execute single compartment simulation of HH model using Forward Euler numerical method" << std::endl;
        return mapp::MAPP_USAGE;
    }
    printHeader ();
    init(v_euler);
    for (t = 0.0; t < end_of_times; t+=delta_t) {
        I = constant_current(t);
        forward_euler(I, v_euler);
        print (t);
    }
    return mapp::MAPP_OK; // 0 ok, 1 not ok
}

/*
 * Neuromapp - helper.h, Copyright (c), 2018,
 * Jeremy Fouriaux - Swiss Federal Institute of technology in Lausanne,
 * jeremy.fouriaux@epfl.ch
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
 * @file neuromapp/cable_section/helper.h
 * \brief Implements the helper function of the solver miniapp
 */

#pragma once

/** \struct input_parameters
    \brief contains the data provides by the user
 */
struct Parameters {
    char* voltage;
    char* current;
    char* capacitance;
    char* vrest;
    int   size;
};

/** \fn print_usage()
    \brief Print the usage of the cstep function
    \return error code MAPP_USAGE
 */
int print_usage();

/** \fn int solver_help(int argc, char * const argv[], struct input_parameters * p)
    \brief Interpret the command line and extract/set up the needed parameter
    \param argc The number of argument in the command line
    \param the command line
    \param p the structure where the input data are saved
    \return may return error code MAPP_BAD_ARG, MAPP_BAD_DATA if the arguments are wrong
 */
int help(int argc, char* const argv[], Parameters * p);



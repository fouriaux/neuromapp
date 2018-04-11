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
 * @file neuromapp/cable_section/helper.cpp
 * \brief Implements the helper function of the kernel miniapp
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

#include "analytical_solutions/helper.h"
#include "utils/error.h"

int print_usage() {
    printf("Usage: cable_section -- [string] --function [string] --data [string] --numthread [int] --name [string]\n");
    printf("Details: \n");
    printf("                 --capacitance [key referencing capacitance data] \n");
    printf("                 --vrest [key referencing VRest data] \n");
    printf("                 --current [key referencing Output current] \n");
    printf("                 --voltage [key referencing voltage output] \n");
    printf("                 --section_size [nb_compartments] \n");
    return MAPP_USAGE;
}


int parse_args(int argc, char * const argv[], struct Parameters * p)
{
  int c;

  p->capacitance = "";
  p->vrest = "";
  p->current = "I_analytic";
  p->voltage = "V_analytic";

  while (1)
  {
      static struct option long_options[] =
      {
          {"help", no_argument, 0, 'h'},
          {"capacitance", required_argument, 0, 'c'},
          {"vrest",  required_argument, 0, 'r'},
          {"current",  required_argument, 0, 'i'},
          {"voltage",  required_argument, 0, 'v'},
          {"section_size", required_argument, 0, 's'},
          {0, 0, 0, 0}
      };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "c:r:i:v:",
                       long_options, &option_index);
      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
      {
          case 'c':
              p->capacitance = optarg;
              break;
          case 'r':
              p->vrest = optarg;
              break;
          case 'i':
              p->current = optarg;
              break;
          case 'v':
              p->voltage = optarg;
              break;
          case 's':
              p->section_size = atoi(optarg);
              break;
          case 'h':
              return kernel_print_usage();
              break;
          default:
              return kernel_print_usage ();
              break;
      }
  }
  return 0 ;
}


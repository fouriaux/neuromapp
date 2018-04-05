/*
 * Neuromapp - main.cpp, Copyright (c), 2015,
 * Fouriaux Jeremy - Swiss Federal Institute of technology in Lausanne,
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
 * @file neuromapp/coreneuron_1.0/simulator/simulator.cpp
 * \brief simulator framework
 */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "utils/plugins/plugin_loader.hpp"


int unknown_command (std::string cmd_name) {
    std::cerr << " unkown command : " << cmd_name << std::endl;
    return CMD_UNKNOWN;
}

int execute (std::vector<std::string> argv) {
    if (! argv.size()) return;
    if (commands.find(argv[0]) == commands.end())
    {
        return unknown_command();
    }
}


/** \brief the main program interacting with the user. The program is command line interactive. */
int main(int argc, char * const argv[]){
// interactive run
    while(1) {
         char* input = readline("");
         add_history(input);
         std::string command(input);
         // in an argv style
         std::vector<std::string> command_v;
         command_v.push_back(argv[0]);
         std::istringstream command_stream(command);
         std::istream_iterator<std::string> wb(command_stream),we;
         std::copy(wb,we,std::back_inserter(command_v));

         if( command_v[1].compare("quit") == 0 ) break;
         execute (command_v);
    }
}

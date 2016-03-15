/*
 * Neuromapp - spike_interface.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/coreneuron_1.0/spike/spike_interface.h
 * \brief Contains spike_interface class declaration.
 */

#ifndef MAPP_SPIKE_INTERFACE_H
#define MAPP_SPIKE_INTERFACE_H

#include <iostream> 

#include "utils/omp/lock.h"

namespace spike {

struct spike_interface{
#ifdef _OPENMP
    mapp::omp_lock lock_;
#else
    mapp::dummy_lock lock_;
#endif

    //CONTAINERS
    std::vector<queueing::event> spikein_;
    std::vector<queueing::event> spikeout_;
    std::vector<int> nin_;
    std::vector<int> displ_;

    spike_interface(int nprocs){ nin_.resize(nprocs); displ_.resize(nprocs); }
};

} //end of namespace

#endif

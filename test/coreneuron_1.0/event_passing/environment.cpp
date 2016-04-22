/*
 * Neuromapp - environment.cpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/coreneuron_1.0/event_passing/environment.cpp
 *  Test on the event passing environment.
 */

#define BOOST_TEST_MODULE enviroTest
#include <boost/test/unit_test.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>
#include <stdlib.h>
#include <time.h>
#include <ctime>

#include "coreneuron_1.0/event_passing/environment/generator.h"
#include "coreneuron_1.0/event_passing/environment/presyn_maker.h"

/**
 * Test the constructor of presyn_maker class
 */
BOOST_AUTO_TEST_CASE(presyns_constructor){
    environment::presyn_maker p1;
    BOOST_CHECK(p1.get_nout() == 0);
    BOOST_CHECK(p1.get_nin() == 0);


    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(1, 10);
    int out = uniform(rng);
    int in = uniform(rng);
    int netcons = uniform(rng);
    environment::presyn_maker p2(out, in, netcons);
    BOOST_CHECK(p2.get_nout() == out);
    BOOST_CHECK(p2.get_nin() == in);
}

/**
 * Test operator() of presyn_maker class
 */
BOOST_AUTO_TEST_CASE(presyns_functor_test){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(5, 10);
    int out = uniform(rng);
    int in = uniform(rng);
    int netcons = uniform(rng);
    environment::presyn_maker p(out, in, netcons);
    int nprocs = 3;
    int ngroups = 3;
    int rank = 0;
    //create presyns
    p(nprocs, ngroups, rank);

    //check for valid output presyns
    bool flag = true;
    for(int i = 0; i < out; ++i){
        if(p[i] != (rank + i)){
            flag = false;
            break;
        }
    }
    BOOST_CHECK(flag);
    //ensure NO invalid input presyns
    environment::input_presyn input;
    for(int i = 0; i < out; ++i){
        if(p.find_input((rank + i),input)){
            flag = false;
            std::cerr<<"Error: found invalid input, "<<input.first<<std::endl;
            break;
        }
    }
    BOOST_CHECK(flag);
}

/**
 * Test the find() function of presyn_maker class
 */
BOOST_AUTO_TEST_CASE(presyns_find_test){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(5, 10);
    int out = uniform(rng);
    int nprocs = 3;
    int ngroups = 3;
    int rank = 0;
    int in = out * (nprocs - 1);
    int netcons = uniform(rng);
    environment::presyn_maker p(out, in, netcons);
    p(nprocs, ngroups, rank);

    bool flag = true;

    environment::input_presyn input;
    //check for valid input presyns
    for(int i = out; i < (out * nprocs); ++i){
        if(!p.find_input(i, input)){
            std::cerr<<"Error: could not find input, "<<i<<std::endl;
            flag = false;
            break;
        }
    }
    BOOST_CHECK(flag);
}

/**
 * Test the constructor of event_generator class
 */
BOOST_AUTO_TEST_CASE(generator_constructor){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(0, 10);
    int nspike = uniform(rng);
    int nlocal = uniform(rng);
    int nite = uniform(rng);
    environment::event_generator generator(nspike, nite, nlocal);

    double sum = static_cast<double>(nspike  + nite+ nlocal);
    BOOST_CHECK_CLOSE(generator.get_sum(), sum, 0.001);
}

/**
 * Test operator() of event_generator class
 */
BOOST_AUTO_TEST_CASE(generator_functor_spike){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(50, 100);
    int nspike = uniform(rng);
    int nite = 0;
    int nlocal = 0;
    int netcons = uniform(rng);
    int out = 10;
    int nprocs = 1;
    int ngroups = 1;
    int simtime = 100;
    int rank = 0;
    int in = out * (nprocs - 1);
    //create presyns
    environment::presyn_maker p(out, in, netcons);
    p(nprocs, ngroups, rank);

    //generate events
    environment::event_generator generator(nspike, nite, nlocal);
    generator(simtime, ngroups, rank, p);

    //check that the number of spike events is close to nspike
/*    double percent_err = static_cast<double>(generator.get_size(0)) /
        static_cast<double>(nspike);
    BOOST_CHECK_close(percent_err, 1.0, 30);
*/

    //check that all events are spike events
    bool spike_type = true;
    environment::gen_event ev;
    BOOST_CHECK(!generator.empty(0));
    while(!generator.empty(0)){
        ev = generator.pop(0);
        if(ev.second != environment::SPIKE){
            spike_type = false;
            break;
        }
    }
    BOOST_CHECK(spike_type);
}

/**
 * Test operator() of event_generator class
 */
BOOST_AUTO_TEST_CASE(generator_check_nonzero){
    boost::mt19937 rng(time(NULL));
    boost::random::uniform_int_distribution<> uniform(5, 10);
    int nspike = uniform(rng);
    int nite = uniform(rng);
    int nlocal = uniform(rng);
    int netcons = uniform(rng);
    int out = 10;
    int nprocs = 1;
    int ngroups = 5;
    int simtime = 100;
    int rank = 0;
    int in = out * (nprocs - 1);
    //create presyns
    environment::presyn_maker p(out, in, netcons);
    p(nprocs, ngroups, rank);

    //generate events
    environment::event_generator generator(nspike, nite, nlocal);
    generator(simtime, ngroups, rank, p);

    //counters
    int spike_c = 0;
    int ite_c = 0;
    int local_c = 0;

    for(int i = 0; i < ngroups; ++i){
        while(!generator.empty(i)){
            environment::gen_event g = generator.pop(i);
            switch(g.second){
            case environment::SPIKE:
                ++spike_c;
                break;
            case environment::ITE:
                ++ite_c;
                break;
            case environment::LOCAL:
                ++local_c;
                break;
            default:
                BOOST_CHECK(false);
            }
        }
    }

    BOOST_CHECK(spike_c != 0);
    BOOST_CHECK(ite_c != 0);
    BOOST_CHECK(local_c != 0);
}

BOOST_AUTO_TEST_CASE(generator_compare_top_lte){
    int nspike = 100;
    int nite = 0;
    int nlocal = 0;
    int netcons = 1;
    int out = 10;
    int nprocs = 1;
    int ngroups = 5;
    int simtime = 100;
    int rank = 0;
    int in = out * (nprocs - 1);
    //create presyns
    environment::presyn_maker p(out, in, netcons);
    p(nprocs, ngroups, rank);

    //generate events
    environment::event_generator generator(nspike, nite, nlocal);
    generator(simtime, ngroups, rank, p);

    bool greater_than_min = true;
    bool less_than_max = true;
    for(int i = 0; i < ngroups; ++i){
        //all events should have a time greater than -1.0
        if(generator.compare_top_lte(i, -1.0)){
            greater_than_min = false;
            environment::gen_event g = generator.pop(i);
            std::cout<<"Event time: "<<g.first.t_<<" =< -1.0"<<std::endl;
            break;
        }
        //all events should have a time less than simtime x 2
        if(!generator.compare_top_lte(i, 200.0)){
            less_than_max = false;
            environment::gen_event g = generator.pop(i);
            std::cout<<"Event time: "<<g.first.t_<<" > 200"<<std::endl;
            break;
        }
    }
    BOOST_CHECK(greater_than_min);
    BOOST_CHECK(less_than_max);
}